import 'dart:typed_data';
import 'package:flutter/material.dart';
import 'package:camera/camera.dart';
import 'package:google_mlkit_barcode_scanning/google_mlkit_barcode_scanning.dart';
import 'package:google_mlkit_commons/google_mlkit_commons.dart';
import 'package:picassowaiter/utils/prefs.dart';

class MLKitBarcodeScanner extends StatefulWidget {
  const MLKitBarcodeScanner({super.key});

  @override
  _MLKitBarcodeScannerState createState() => _MLKitBarcodeScannerState();
}

class _MLKitBarcodeScannerState extends State<MLKitBarcodeScanner> {
  CameraController? _cameraController;
  late BarcodeScanner _barcodeScanner;
  bool _isClosed = false;
  String? _scannedCode;
  DateTime? _lastProcessTime;

  final double scanWindowSize = 250.0;

  @override
  void initState() {
    super.initState();
    print(
      'MLKitBarcodeScanner MLKitBarcodeScanner MLKitBarcodeScanner MLKitBarcodeScanner',
    );
    _barcodeScanner = BarcodeScanner(
      formats: [
        BarcodeFormat.qrCode,
        BarcodeFormat.dataMatrix,
        BarcodeFormat.ean8,
        BarcodeFormat.ean13,
      ],
    );
    _initCamera();
  }

  Future<void> _initCamera() async {
    final cameras = await availableCameras();
    final backCamera = cameras.firstWhere(
      (c) => c.lensDirection == CameraLensDirection.back,
    );

    _cameraController = CameraController(
      backCamera,
      ResolutionPreset.medium,
      enableAudio: false,
      imageFormatGroup: ImageFormatGroup.yuv420, // важный параметр
    );

    await _cameraController?.initialize();
    await _cameraController?.startImageStream(_processCameraImage);

    if (mounted) setState(() {});
  }

  Future<void> _processCameraImage(CameraImage image) async {
    if (_isClosed){ return; }

    if (_lastProcessTime != null &&
        DateTime.now().difference(_lastProcessTime!).inMilliseconds < 100) {
      return;
    }
    _lastProcessTime = DateTime.now();

    try {
      final bytes = _yuv420ToNv21(image);
      final rotation = InputImageRotationValue.fromRawValue(
          _cameraController!.description.sensorOrientation) ??
          InputImageRotation.rotation0deg;
      final inputImage = InputImage.fromBytes(
        bytes: bytes,
        metadata: InputImageMetadata(
          size: Size(image.width.toDouble(), image.height.toDouble()),
          rotation: rotation,
          format: InputImageFormat.nv21, // для большинства устройств
          bytesPerRow: image.planes[0].bytesPerRow,
        ),
      );

      // final inputImage = InputImage.fromBytes(
      //   bytes: bytes,
      //   metadata: InputImageMetadata(
      //     size: Size(image.width.toDouble(), image.height.toDouble()),
      //     rotation: InputImageRotation.rotation0deg,
      //     format: InputImageFormat.nv21,
      //     bytesPerRow: image.planes[0].bytesPerRow,
      //   ),
      // );

      final barcodes = await _barcodeScanner.processImage(inputImage);

      if (barcodes.isEmpty) { return;}

      final centerX = image.width / 2;
      final centerY = image.height / 2;
      final halfSize = scanWindowSize / 2;
      final scanRect = Rect.fromLTWH(
        centerX - halfSize,
        centerY - halfSize,
        scanWindowSize,
        scanWindowSize,
      );

      final filtered = barcodes.where((b) {
        final box = b.boundingBox;
        if (box == null) return false;
        return scanRect.overlaps(box);
      }).toList();

      if (filtered.isNotEmpty && mounted) {
        // Блокируем повторный вызов
        _isClosed = true;

        // Очищаем и сохраняем код
        _scannedCode = filtered.first.displayValue?.trim().replaceAll(
          RegExp(r'^[\x00-\x1F]+'),
          '',
        );
        print('SCANNNNED CODE $_scannedCode');

        if (mounted) {
          await _barcodeScanner.close();
          await _cameraController?.stopImageStream();
          Navigator.of(prefs.context()).pop(_scannedCode);
        }
      }
    } catch (e, st) {
      print('Barcode scan error: $e\n$st');
    }
  }

  Uint8List _yuv420ToNv21(CameraImage image) {
    final width = image.width;
    final height = image.height;

    final ySize = width * height;
    final uvSize = width * height ~/ 2;
    final nv21 = Uint8List(ySize + uvSize);

    // Y
    int offset = 0;
    for (final plane in image.planes) {
      if (plane.bytesPerPixel == 1) { // это Y
        for (int row = 0; row < height; row++) {
          final rowData = plane.bytes.sublist(
            row * plane.bytesPerRow,
            row * plane.bytesPerRow + width,
          );
          nv21.setRange(offset, offset + width, rowData);
          offset += width;
        }
        break;
      }
    }

    // U + V (через interleave)
    final u = image.planes[1];
    final v = image.planes[2];

    final uvRowStride = u.bytesPerRow;
    final uvPixelStride = u.bytesPerPixel!;

    int uvIndex = ySize;
    for (int row = 0; row < height ~/ 2; row++) {
      for (int col = 0; col < width ~/ 2; col++) {
        final uIndex = row * uvRowStride + col * uvPixelStride;
        final vIndex = row * v.bytesPerRow + col * v.bytesPerPixel!;

        nv21[uvIndex++] = v.bytes[vIndex];
        nv21[uvIndex++] = u.bytes[uIndex];
      }
    }

    return nv21;
  }

  Uint8List _concatenatePlanesToNV21(CameraImage image) {
    final width = image.width;
    final height = image.height;

    final ySize = width * height;
    final uvSize = width * height ~/ 2;

    final nv21 = Uint8List(ySize + uvSize);

    // копируем Y как есть
    nv21.setRange(0, ySize, image.planes[0].bytes);

    // получаем ссылки на U и V
    final u = image.planes[1].bytes;
    final v = image.planes[2].bytes;

    final uvRowStride = image.planes[1].bytesPerRow;
    final uvPixelStride = image.planes[1].bytesPerPixel!; // обычно = 2

    int uvIndex = ySize;

    // итерируемся по каждому пикселю UV-плоскости
    for (int row = 0; row < height ~/ 2; row++) {
      for (int col = 0; col < width ~/ 2; col++) {
        final uIndex = row * uvRowStride + col * uvPixelStride;
        final vIndex = row * image.planes[2].bytesPerRow + col * image.planes[2].bytesPerPixel!;

        nv21[uvIndex++] = v[vIndex];
        nv21[uvIndex++] = u[uIndex];
      }
    }

    return nv21;
  }



  Uint8List _concatenatePlanes(CameraImage image) {
    final bytes = <int>[];
    for (final plane in image.planes) {
      bytes.addAll(plane.bytes);
    }
    return Uint8List.fromList(bytes);
  }

  @override
  Widget build(BuildContext context) {
    if (!(_cameraController?.value.isInitialized ?? false)) {
      return const Scaffold(body: Center(child: CircularProgressIndicator()));
    }

    final scanWindow = Rect.fromCenter(
      center: MediaQuery.sizeOf(context).center(Offset.zero),
      width: scanWindowSize,
      height: scanWindowSize,
    );

    return Scaffold(
      backgroundColor: Colors.black,
      appBar: AppBar(title: Text('Barcode scanner ${prefs.getString('appbuild')}')),
      body: Stack(
        fit: StackFit.expand,
        children: [
          CameraPreview(_cameraController!),
          CustomPaint(painter: ScannerOverlay(scanWindow: scanWindow)),
          if (_scannedCode != null)
            Positioned(
              bottom: 32,
              left: 16,
              right: 16,
              child: Text(
                _scannedCode!,
                style: const TextStyle(
                  color: Colors.white,
                  fontSize: 18,
                  fontWeight: FontWeight.bold,
                ),
                textAlign: TextAlign.center,
              ),
            ),
        ],
      ),
    );
  }

  @override
  void dispose() {
    _cameraController?.dispose();
    _barcodeScanner.close();
    super.dispose();
  }
}

class ScannerOverlay extends CustomPainter {
  final Rect scanWindow;
  final double borderRadius;

  ScannerOverlay({required this.scanWindow, this.borderRadius = 12.0});

  @override
  void paint(Canvas canvas, Size size) {
    final backgroundPath = Path()..addRect(Rect.largest);

    final cutoutPath = Path()
      ..addRRect(
        RRect.fromRectAndCorners(
          scanWindow,
          topLeft: Radius.circular(borderRadius),
          topRight: Radius.circular(borderRadius),
          bottomLeft: Radius.circular(borderRadius),
          bottomRight: Radius.circular(borderRadius),
        ),
      );

    final backgroundPaint = Paint()
      ..color = Colors.black.withOpacity(0.5)
      ..style = PaintingStyle.fill
      ..blendMode = BlendMode.dstOut;

    final backgroundWithCutout = Path.combine(
      PathOperation.difference,
      backgroundPath,
      cutoutPath,
    );

    final borderPaint = Paint()
      ..color = Colors.greenAccent
      ..style = PaintingStyle.stroke
      ..strokeWidth = 3.0;

    canvas.drawPath(backgroundWithCutout, backgroundPaint);
    canvas.drawRRect(
      RRect.fromRectAndCorners(
        scanWindow,
        topLeft: Radius.circular(borderRadius),
        topRight: Radius.circular(borderRadius),
        bottomLeft: Radius.circular(borderRadius),
        bottomRight: Radius.circular(borderRadius),
      ),
      borderPaint,
    );
  }

  @override
  bool shouldRepaint(covariant ScannerOverlay oldDelegate) =>
      oldDelegate.scanWindow != scanWindow ||
      oldDelegate.borderRadius != borderRadius;
}
