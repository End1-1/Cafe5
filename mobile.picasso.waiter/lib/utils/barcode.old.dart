// import 'dart:async';
//
// import 'package:picassowaiter/utils/prefs.dart';
// import 'package:flutter/material.dart';
// import 'package:mobile_scanner/mobile_scanner.dart';
//
// class BarcodeScannerWithOverlay extends StatefulWidget {
//   const BarcodeScannerWithOverlay({super.key});
//
//   @override
//   _BarcodeScannerWithOverlayState createState() =>
//       _BarcodeScannerWithOverlayState();
// }
//
// class _BarcodeScannerWithOverlayState extends State<BarcodeScannerWithOverlay> {
//   final MobileScannerController controller = MobileScannerController(
//     detectionSpeed: DetectionSpeed.noDuplicates,
//     formats: const [BarcodeFormat.dataMatrix, BarcodeFormat.qrCode, BarcodeFormat.ean8, BarcodeFormat.ean13],
//   );
//
//   late final StreamSubscription<BarcodeCapture> f;
//
//
//   @override
//   void initState() {
//     super.initState();
//     f = controller.barcodes.listen((event) {
//       if (event.barcodes.isNotEmpty) {
//         f.cancel();
//         Navigator.pop(prefs.context(), event.barcodes.first.rawValue);
//       }
//     });
//   }
//
//   @override
//   Widget build(BuildContext context) {
//     final scanWindow = Rect.fromCenter(
//       center: MediaQuery.sizeOf(context).center(Offset.zero),
//       width: 200,
//       height: 200,
//     );
//
//     return Scaffold(
//       backgroundColor: Colors.black,
//       appBar: AppBar(
//         title: const Text(''),
//       ),
//       body: Stack(
//         fit: StackFit.expand,
//         children: [
//           Center(
//             child: MobileScanner(
//               fit: BoxFit.contain,
//               controller: controller,
//               scanWindow: scanWindow,
//               errorBuilder: (context, error) {
//                 return Text('$error');
//               },
//               overlayBuilder: (context, constraints) {
//                 return Padding(
//                   padding: const EdgeInsets.all(16.0),
//                   child: Align(
//                     alignment: Alignment.bottomCenter,
//                     child: ScannedBarcodeLabel(barcodes: controller.barcodes),
//                   ),
//                 );
//               },
//             ),
//           ),
//           ValueListenableBuilder(
//             valueListenable: controller,
//             builder: (context, value, child) {
//               if (!value.isInitialized ||
//                   !value.isRunning ||
//                   value.error != null) {
//                 return const SizedBox();
//               }
//
//               return CustomPaint(
//                 painter: ScannerOverlay(scanWindow: scanWindow),
//               );
//             },
//           )
//         ],
//       ),
//     );
//   }
//
//   @override
//   Future<void> dispose() async {
//     super.dispose();
//     await controller.dispose();
//   }
// }
//
// class ScannerOverlay extends CustomPainter {
//   const ScannerOverlay({
//     required this.scanWindow,
//     this.borderRadius = 12.0,
//   });
//
//   final Rect scanWindow;
//   final double borderRadius;
//
//   @override
//   void paint(Canvas canvas, Size size) {
//     // TODO: use `Offset.zero & size` instead of Rect.largest
//     // we need to pass the size to the custom paint widget
//     final backgroundPath = Path()..addRect(Rect.largest);
//
//     final cutoutPath = Path()
//       ..addRRect(
//         RRect.fromRectAndCorners(
//           scanWindow,
//           topLeft: Radius.circular(borderRadius),
//           topRight: Radius.circular(borderRadius),
//           bottomLeft: Radius.circular(borderRadius),
//           bottomRight: Radius.circular(borderRadius),
//         ),
//       );
//
//     final backgroundPaint = Paint()
//       ..color = Colors.black.withOpacity(0.5)
//       ..style = PaintingStyle.fill
//       ..blendMode = BlendMode.dstOut;
//
//     final backgroundWithCutout = Path.combine(
//       PathOperation.difference,
//       backgroundPath,
//       cutoutPath,
//     );
//
//     final borderPaint = Paint()
//       ..color = Colors.white
//       ..style = PaintingStyle.stroke
//       ..strokeWidth = 4.0;
//
//     final borderRect = RRect.fromRectAndCorners(
//       scanWindow,
//       topLeft: Radius.circular(borderRadius),
//       topRight: Radius.circular(borderRadius),
//       bottomLeft: Radius.circular(borderRadius),
//       bottomRight: Radius.circular(borderRadius),
//     );
//
//     // First, draw the background,
//     // with a cutout area that is a bit larger than the scan window.
//     // Finally, draw the scan window itself.
//     canvas.drawPath(backgroundWithCutout, backgroundPaint);
//     canvas.drawRRect(borderRect, borderPaint);
//   }
//
//   @override
//   bool shouldRepaint(ScannerOverlay oldDelegate) {
//     return scanWindow != oldDelegate.scanWindow ||
//         borderRadius != oldDelegate.borderRadius;
//   }
// }
//
// class ScannedBarcodeLabel extends StatelessWidget {
//   const ScannedBarcodeLabel({
//     super.key,
//     required this.barcodes,
//   });
//
//   final Stream<BarcodeCapture> barcodes;
//
//   @override
//   Widget build(BuildContext context) {
//     return StreamBuilder(
//       stream: barcodes,
//       builder: (context, snapshot) {
//         final scannedBarcodes = snapshot.data?.barcodes ?? [];
//
//         if (scannedBarcodes.isEmpty) {
//           return const Text(
//             'Scan something!',
//             overflow: TextOverflow.fade,
//             style: TextStyle(color: Colors.white),
//           );
//         }
//
//         return Text(
//           scannedBarcodes.first.displayValue ?? 'No display value.',
//           overflow: TextOverflow.fade,
//           style: const TextStyle(color: Colors.white),
//         );
//       },
//     );
//   }
// }
//
