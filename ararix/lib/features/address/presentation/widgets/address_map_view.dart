import 'package:flutter/foundation.dart';
import 'package:flutter/gestures.dart';
import 'package:flutter/material.dart';
import 'package:google_maps_flutter/google_maps_flutter.dart';

import '../../data/maps_config.dart';

/// Map with green house pin. Falls back to a placeholder when Maps key is missing.
class AddressMapView extends StatelessWidget {
  const AddressMapView({
    super.key,
    required this.lat,
    required this.lng,
    this.callout,
    this.onCameraIdle,
    this.draggable = true,
    this.height = 220,
    this.myLocationEnabled = false,
  });

  final double lat;
  final double lng;
  final String? callout;
  final ValueChanged<LatLng>? onCameraIdle;
  final bool draggable;
  final double height;
  final bool myLocationEnabled;

  @override
  Widget build(BuildContext context) {
    return SizedBox(
      height: height,
      width: double.infinity,
      child: ClipRRect(
        borderRadius: BorderRadius.circular(16),
        child: Stack(
          children: [
            Positioned.fill(
              child: MapsConfig.isConfigured
                  ? _GoogleMapBody(
                      lat: lat,
                      lng: lng,
                      onCameraIdle: onCameraIdle,
                      draggable: draggable,
                      myLocationEnabled: myLocationEnabled,
                    )
                  : _PlaceholderMap(
                      lat: lat,
                      lng: lng,
                      onTapOffset: onCameraIdle == null
                          ? null
                          : (dx, dy) {
                              final next = LatLng(
                                lat - dy * 0.00008,
                                lng + dx * 0.00008,
                              );
                              onCameraIdle!(next);
                            },
                    ),
            ),
            IgnorePointer(
              child: Center(
                child: Column(
                  mainAxisSize: MainAxisSize.min,
                  children: [
                    if (callout != null && callout!.trim().isNotEmpty)
                      Container(
                        margin: const EdgeInsets.only(bottom: 6),
                        padding: const EdgeInsets.symmetric(
                          horizontal: 10,
                          vertical: 6,
                        ),
                        constraints: const BoxConstraints(maxWidth: 220),
                        decoration: BoxDecoration(
                          color: Colors.white,
                          borderRadius: BorderRadius.circular(8),
                          boxShadow: [
                            BoxShadow(
                              color: Colors.black.withValues(alpha: 0.12),
                              blurRadius: 8,
                              offset: const Offset(0, 2),
                            ),
                          ],
                        ),
                        child: Text(
                          callout!,
                          textAlign: TextAlign.center,
                          maxLines: 2,
                          overflow: TextOverflow.ellipsis,
                          style: const TextStyle(
                            fontSize: 12,
                            fontWeight: FontWeight.w600,
                          ),
                        ),
                      ),
                    const _HousePin(),
                  ],
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}

class _GoogleMapBody extends StatefulWidget {
  const _GoogleMapBody({
    required this.lat,
    required this.lng,
    this.onCameraIdle,
    required this.draggable,
    required this.myLocationEnabled,
  });

  final double lat;
  final double lng;
  final ValueChanged<LatLng>? onCameraIdle;
  final bool draggable;
  final bool myLocationEnabled;

  @override
  State<_GoogleMapBody> createState() => _GoogleMapBodyState();
}

class _GoogleMapBodyState extends State<_GoogleMapBody> {
  static final _gestureRecognizers = <Factory<OneSequenceGestureRecognizer>>{
    Factory<OneSequenceGestureRecognizer>(() => EagerGestureRecognizer()),
  };

  GoogleMapController? _controller;
  bool _movingProgrammatically = false;
  LatLng? _reportedCenter;

  bool _samePoint(double aLat, double aLng, double bLat, double bLng) {
    return (aLat - bLat).abs() < 1e-6 && (aLng - bLng).abs() < 1e-6;
  }

  @override
  void didUpdateWidget(covariant _GoogleMapBody oldWidget) {
    super.didUpdateWidget(oldWidget);
    final reported = _reportedCenter;
    if (reported != null &&
        _samePoint(widget.lat, widget.lng, reported.latitude, reported.longitude)) {
      return;
    }
    if ((oldWidget.lat - widget.lat).abs() > 1e-7 ||
        (oldWidget.lng - widget.lng).abs() > 1e-7) {
      _movingProgrammatically = true;
      _controller?.animateCamera(
        CameraUpdate.newLatLng(LatLng(widget.lat, widget.lng)),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    return GoogleMap(
      initialCameraPosition: CameraPosition(
        target: LatLng(widget.lat, widget.lng),
        zoom: 16,
      ),
      gestureRecognizers: _gestureRecognizers,
      myLocationEnabled: widget.myLocationEnabled,
      myLocationButtonEnabled: false,
      zoomControlsEnabled: false,
      compassEnabled: false,
      mapToolbarEnabled: false,
      scrollGesturesEnabled: widget.draggable,
      zoomGesturesEnabled: widget.draggable,
      tiltGesturesEnabled: false,
      rotateGesturesEnabled: false,
      onMapCreated: (c) => _controller = c,
      onCameraIdle: () async {
        if (_movingProgrammatically) {
          _movingProgrammatically = false;
          return;
        }
        if (widget.onCameraIdle == null || _controller == null) return;
        final bounds = await _controller!.getVisibleRegion();
        final center = LatLng(
          (bounds.northeast.latitude + bounds.southwest.latitude) / 2,
          (bounds.northeast.longitude + bounds.southwest.longitude) / 2,
        );
        _reportedCenter = center;
        widget.onCameraIdle!(center);
      },
    );
  }
}

class _PlaceholderMap extends StatelessWidget {
  const _PlaceholderMap({
    required this.lat,
    required this.lng,
    this.onTapOffset,
  });

  final double lat;
  final double lng;
  final void Function(double dx, double dy)? onTapOffset;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onPanUpdate: onTapOffset == null
          ? null
          : (d) => onTapOffset!(d.delta.dx, d.delta.dy),
      child: Container(
        decoration: const BoxDecoration(
          gradient: LinearGradient(
            begin: Alignment.topLeft,
            end: Alignment.bottomRight,
            colors: [Color(0xFFDDE8DE), Color(0xFFC5D5C8)],
          ),
        ),
        child: Stack(
          children: [
            Positioned(
              left: 12,
              bottom: 12,
              child: Text(
                '${lat.toStringAsFixed(5)}, ${lng.toStringAsFixed(5)}',
                style: TextStyle(
                  fontSize: 11,
                  color: Colors.black.withValues(alpha: 0.45),
                ),
              ),
            ),
            Positioned(
              right: 12,
              top: 12,
              child: Container(
                padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 4),
                decoration: BoxDecoration(
                  color: Colors.white.withValues(alpha: 0.85),
                  borderRadius: BorderRadius.circular(8),
                ),
                child: const Text(
                  'Map key pending',
                  style: TextStyle(fontSize: 11, fontWeight: FontWeight.w600),
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}

class _HousePin extends StatelessWidget {
  const _HousePin();

  @override
  Widget build(BuildContext context) {
    return Column(
      mainAxisSize: MainAxisSize.min,
      children: [
        Container(
          width: 44,
          height: 44,
          decoration: BoxDecoration(
            color: const Color(0xFF2EAE57),
            shape: BoxShape.circle,
            border: Border.all(color: Colors.white, width: 3),
            boxShadow: [
              BoxShadow(
                color: Colors.black.withValues(alpha: 0.2),
                blurRadius: 8,
                offset: const Offset(0, 3),
              ),
            ],
          ),
          child: const Icon(Icons.home_rounded, color: Colors.white, size: 22),
        ),
        CustomPaint(
          size: const Size(12, 8),
          painter: _PinTailPainter(),
        ),
      ],
    );
  }
}

class _PinTailPainter extends CustomPainter {
  @override
  void paint(Canvas canvas, Size size) {
    final paint = Paint()..color = const Color(0xFF2EAE57);
    final path = Path()
      ..moveTo(0, 0)
      ..lineTo(size.width / 2, size.height)
      ..lineTo(size.width, 0)
      ..close();
    canvas.drawPath(path, paint);
  }

  @override
  bool shouldRepaint(covariant CustomPainter oldDelegate) => false;
}
