import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';

import '../../../l10n/app_localizations.dart';
import '../application/address_notifier.dart';
import '../data/address_models.dart';
import 'widgets/address_map_view.dart';

Future<bool> showAddressEditor(
  BuildContext context,
  WidgetRef ref, {
  DeliveryAddress? existing,
}) async {
  final notifier = ref.read(addressProvider.notifier);
  if (existing != null) {
    notifier.editExisting(existing);
  } else {
    notifier.startNewAddress();
  }
  final result = await showModalBottomSheet<bool>(
    context: context,
    isScrollControlled: true,
    useSafeArea: true,
    backgroundColor: Colors.white,
    shape: const RoundedRectangleBorder(
      borderRadius: BorderRadius.vertical(top: Radius.circular(20)),
    ),
    builder: (_) => const AddressFlowSheet(),
  );
  return result == true;
}

Future<bool> showAddressPicker(BuildContext context, WidgetRef ref) async {
  ref.read(addressProvider.notifier).openWhere();
  final result = await showModalBottomSheet<bool>(
    context: context,
    isScrollControlled: true,
    useSafeArea: true,
    backgroundColor: Colors.white,
    shape: const RoundedRectangleBorder(
      borderRadius: BorderRadius.vertical(top: Radius.circular(20)),
    ),
    builder: (_) => const AddressFlowSheet(),
  );
  return result == true;
}

class AddressFlowSheet extends ConsumerWidget {
  const AddressFlowSheet({super.key});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final step = ref.watch(addressProvider.select((s) => s.step));
    final height = MediaQuery.sizeOf(context).height * 0.92;

    return SizedBox(
      height: height,
      child: AnimatedSwitcher(
        duration: const Duration(milliseconds: 220),
        child: switch (step) {
          AddressFlowStep.where => const _WhereStep(key: ValueKey('where')),
          AddressFlowStep.search => const _SearchStep(key: ValueKey('search')),
          AddressFlowStep.confirm =>
            const _ConfirmStep(key: ValueKey('confirm')),
          AddressFlowStep.buildingType =>
            const _BuildingTypeStep(key: ValueKey('building')),
          AddressFlowStep.details =>
            const _DetailsStep(key: ValueKey('details')),
        },
      ),
    );
  }
}

class _SheetHeader extends StatelessWidget {
  const _SheetHeader({
    required this.title,
    this.onClose,
    this.onBack,
  });

  final String title;
  final VoidCallback? onClose;
  final VoidCallback? onBack;

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.fromLTRB(8, 8, 8, 4),
      child: Row(
        children: [
          if (onBack != null)
            IconButton(
              onPressed: onBack,
              icon: const Icon(Icons.arrow_back),
            )
          else
            const SizedBox(width: 48),
          Expanded(
            child: Text(
              title,
              textAlign: TextAlign.center,
              style: const TextStyle(fontSize: 18, fontWeight: FontWeight.w700),
            ),
          ),
          if (onClose != null)
            IconButton(
              onPressed: onClose,
              icon: const Icon(Icons.close),
            )
          else
            const SizedBox(width: 48),
        ],
      ),
    );
  }
}

class _WhereStep extends ConsumerWidget {
  const _WhereStep({super.key});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final l10n = AppLocalizations.of(context);
    final state = ref.watch(addressProvider);
    final notifier = ref.read(addressProvider.notifier);
    final preview = state.active;

    return Column(
      children: [
        _SheetHeader(
          title: l10n.whereDeliver,
          onClose: () => Navigator.of(context).pop(false),
        ),
        if (state.error != null)
          Padding(
            padding: const EdgeInsets.symmetric(horizontal: 16),
            child: Text(state.error!, style: const TextStyle(color: Colors.red)),
          ),
        Expanded(
          child: ListView(
            padding: const EdgeInsets.fromLTRB(16, 8, 16, 24),
            children: [
              ListTile(
                contentPadding: EdgeInsets.zero,
                leading: const Icon(Icons.my_location_outlined),
                title: Text(l10n.currentLocation),
                onTap: state.saving
                    ? null
                    : () => notifier.useCurrentLocation(),
              ),
              const Divider(),
              if (state.loading)
                const Padding(
                  padding: EdgeInsets.all(24),
                  child: Center(child: CircularProgressIndicator()),
                )
              else ...[
                for (final a in state.addresses)
                  ListTile(
                    contentPadding: EdgeInsets.zero,
                    leading: Icon(
                      a.isActive
                          ? Icons.radio_button_checked
                          : Icons.radio_button_off,
                      color: a.isActive ? const Color(0xFF2EAE57) : null,
                    ),
                    title: Text(
                      a.displayLabel,
                      maxLines: 1,
                      overflow: TextOverflow.ellipsis,
                    ),
                    subtitle: a.street != a.label
                        ? Text(
                            a.street,
                            maxLines: 1,
                            overflow: TextOverflow.ellipsis,
                          )
                        : null,
                    trailing: IconButton(
                      icon: const Icon(Icons.edit_outlined),
                      onPressed: () => notifier.editExisting(a),
                    ),
                    onTap: state.saving
                        ? null
                        : () async {
                            final ok = await notifier.selectSaved(a);
                            if (ok && context.mounted) {
                              Navigator.of(context).pop(true);
                            }
                          },
                  ),
              ],
              const SizedBox(height: 8),
              OutlinedButton.icon(
                onPressed: notifier.goSearch,
                icon: const Icon(Icons.add),
                label: Text(l10n.addNewAddress),
              ),
              const SizedBox(height: 16),
              AddressMapView(
                lat: preview?.lat ?? state.draft.lat,
                lng: preview?.lng ?? state.draft.lng,
                callout: preview?.displayLabel.isNotEmpty == true
                    ? preview!.displayLabel
                    : null,
                draggable: false,
                height: 200,
              ),
              const SizedBox(height: 12),
              TextButton.icon(
                onPressed: state.saving
                    ? null
                    : () => notifier.useCurrentLocation(),
                icon: const Icon(Icons.near_me_outlined),
                label: Text(l10n.locateMe),
              ),
            ],
          ),
        ),
      ],
    );
  }
}

class _SearchStep extends ConsumerWidget {
  const _SearchStep({super.key});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final l10n = AppLocalizations.of(context);
    final state = ref.watch(addressProvider);
    final notifier = ref.read(addressProvider.notifier);

    return Column(
      children: [
        _SheetHeader(
          title: l10n.searchAddressTitle,
          onBack: notifier.back,
          onClose: () => Navigator.of(context).pop(false),
        ),
        Padding(
          padding: const EdgeInsets.fromLTRB(16, 0, 16, 8),
          child: TextField(
            autofocus: true,
            onChanged: notifier.setSearchQuery,
            decoration: InputDecoration(
              hintText: l10n.enterStreetBuilding,
              prefixIcon: const Icon(Icons.search),
              border: OutlineInputBorder(
                borderRadius: BorderRadius.circular(12),
              ),
            ),
          ),
        ),
        if (state.suggesting)
          const LinearProgressIndicator(minHeight: 2),
        Expanded(
          child: ListView.separated(
            itemCount: state.suggestions.length,
            separatorBuilder: (_, _) => const Divider(height: 1),
            itemBuilder: (context, i) {
              final s = state.suggestions[i];
              return ListTile(
                leading: const Icon(Icons.place_outlined),
                title: Text(s.primaryText),
                subtitle: s.secondaryText.isEmpty
                    ? null
                    : Text(s.secondaryText),
                onTap: () => notifier.pickSuggestion(s),
              );
            },
          ),
        ),
      ],
    );
  }
}

class _ConfirmStep extends ConsumerWidget {
  const _ConfirmStep({super.key});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final l10n = AppLocalizations.of(context);
    final draft = ref.watch(addressProvider.select((s) => s.draft));
    final notifier = ref.read(addressProvider.notifier);

    return Column(
      children: [
        _SheetHeader(
          title: l10n.confirmAddress,
          onBack: notifier.back,
          onClose: () => Navigator.of(context).pop(false),
        ),
        Expanded(
          child: Padding(
            padding: const EdgeInsets.fromLTRB(16, 0, 16, 16),
            child: Column(
              children: [
                Expanded(
                  child: AddressMapView(
                    lat: draft.lat,
                    lng: draft.lng,
                    callout: draft.street.isNotEmpty ? draft.street : null,
                    height: double.infinity,
                    onCameraIdle: (ll) {
                      notifier.updatePin(ll.latitude, ll.longitude);
                    },
                  ),
                ),
                const SizedBox(height: 12),
                Align(
                  alignment: Alignment.centerLeft,
                  child: Text(
                    draft.street.isNotEmpty
                        ? draft.street
                        : l10n.droppedPin,
                    style: const TextStyle(
                      fontSize: 16,
                      fontWeight: FontWeight.w600,
                    ),
                  ),
                ),
                const SizedBox(height: 12),
                SizedBox(
                  width: double.infinity,
                  height: 48,
                  child: FilledButton(
                    style: FilledButton.styleFrom(
                      backgroundColor: const Color(0xFF2EAE57),
                    ),
                    onPressed: notifier.goBuildingType,
                    child: Text(l10n.confirmAddress),
                  ),
                ),
              ],
            ),
          ),
        ),
      ],
    );
  }
}

class _BuildingTypeStep extends ConsumerWidget {
  const _BuildingTypeStep({super.key});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final l10n = AppLocalizations.of(context);
    final selected =
        ref.watch(addressProvider.select((s) => s.draft.buildingType));
    final notifier = ref.read(addressProvider.notifier);

    final items = <(BuildingType, String, IconData)>[
      (BuildingType.house, l10n.buildingHouse, Icons.home_outlined),
      (BuildingType.apartment, l10n.buildingApartment, Icons.apartment),
      (BuildingType.office, l10n.buildingOffice, Icons.business_outlined),
      (BuildingType.other, l10n.buildingOther, Icons.place_outlined),
    ];

    return Column(
      children: [
        _SheetHeader(
          title: l10n.buildingTypeTitle,
          onBack: notifier.back,
          onClose: () => Navigator.of(context).pop(false),
        ),
        Expanded(
          child: Padding(
            padding: const EdgeInsets.all(16),
            child: GridView.count(
              crossAxisCount: 2,
              mainAxisSpacing: 12,
              crossAxisSpacing: 12,
              childAspectRatio: 1.15,
              children: [
                for (final item in items)
                  _BuildingTypeTile(
                    label: item.$2,
                    icon: item.$3,
                    selected: selected == item.$1,
                    onTap: () {
                      notifier.setBuildingType(item.$1);
                      notifier.goDetails();
                    },
                  ),
              ],
            ),
          ),
        ),
      ],
    );
  }
}

class _BuildingTypeTile extends StatelessWidget {
  const _BuildingTypeTile({
    required this.label,
    required this.icon,
    required this.selected,
    required this.onTap,
  });

  final String label;
  final IconData icon;
  final bool selected;
  final VoidCallback onTap;

  @override
  Widget build(BuildContext context) {
    return Material(
      color: Colors.white,
      borderRadius: BorderRadius.circular(14),
      child: InkWell(
        onTap: onTap,
        borderRadius: BorderRadius.circular(14),
        child: Container(
          decoration: BoxDecoration(
            borderRadius: BorderRadius.circular(14),
            border: Border.all(
              color: selected ? const Color(0xFF1A1A1A) : const Color(0xFFDDDDDD),
              width: selected ? 2.5 : 1,
            ),
          ),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Icon(icon, size: 36),
              const SizedBox(height: 10),
              Text(
                label,
                style: TextStyle(
                  fontWeight: selected ? FontWeight.w700 : FontWeight.w500,
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}

class _DetailsStep extends ConsumerStatefulWidget {
  const _DetailsStep({super.key});

  @override
  ConsumerState<_DetailsStep> createState() => _DetailsStepState();
}

class _DetailsStepState extends ConsumerState<_DetailsStep> {
  late final TextEditingController _floor;
  late final TextEditingController _door;
  late final TextEditingController _comment;
  late final TextEditingController _label;

  @override
  void initState() {
    super.initState();
    final d = ref.read(addressProvider).draft;
    _floor = TextEditingController(text: d.floor);
    _door = TextEditingController(text: d.door);
    _comment = TextEditingController(text: d.comment);
    _label = TextEditingController(text: d.label);
  }

  @override
  void dispose() {
    _floor.dispose();
    _door.dispose();
    _comment.dispose();
    _label.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final l10n = AppLocalizations.of(context);
    final state = ref.watch(addressProvider);
    final draft = state.draft;
    final notifier = ref.read(addressProvider.notifier);

    return Column(
      children: [
        _SheetHeader(
          title: l10n.addressDetailsTitle,
          onBack: notifier.back,
          onClose: () => Navigator.of(context).pop(false),
        ),
        Expanded(
          child: ListView(
            padding: const EdgeInsets.fromLTRB(16, 0, 16, 24),
            children: [
              Row(
                children: [
                  Expanded(
                    child: TextField(
                      controller: _floor,
                      onChanged: notifier.setFloor,
                      decoration: InputDecoration(
                        labelText: l10n.floor,
                        border: const OutlineInputBorder(),
                      ),
                    ),
                  ),
                  const SizedBox(width: 12),
                  Expanded(
                    child: TextField(
                      controller: _door,
                      onChanged: notifier.setDoor,
                      decoration: InputDecoration(
                        labelText: l10n.door,
                        border: const OutlineInputBorder(),
                      ),
                    ),
                  ),
                ],
              ),
              const SizedBox(height: 12),
              TextField(
                controller: _comment,
                onChanged: notifier.setComment,
                maxLines: 3,
                decoration: InputDecoration(
                  labelText: l10n.additionalInfo,
                  border: const OutlineInputBorder(),
                ),
              ),
              const SizedBox(height: 16),
              Text(
                l10n.markEntrance,
                style: const TextStyle(fontWeight: FontWeight.w700),
              ),
              const SizedBox(height: 8),
              AddressMapView(
                lat: draft.entranceLat ?? draft.lat,
                lng: draft.entranceLng ?? draft.lng,
                callout: l10n.entrance,
                height: 160,
                onCameraIdle: (ll) {
                  notifier.updateEntrancePin(ll.latitude, ll.longitude);
                },
              ),
              const SizedBox(height: 12),
              Text(
                l10n.addressLabel,
                style: const TextStyle(fontWeight: FontWeight.w700),
              ),
              const SizedBox(height: 8),
              Wrap(
                spacing: 8,
                children: [
                  for (final label in [l10n.labelHome, l10n.labelWork, l10n.labelOther])
                    ChoiceChip(
                      label: Text(label),
                      selected: draft.label == label,
                      onSelected: (_) {
                        _label.text = label;
                        notifier.setLabel(label);
                      },
                    ),
                ],
              ),
              const SizedBox(height: 8),
              TextField(
                controller: _label,
                onChanged: notifier.setLabel,
                decoration: InputDecoration(
                  labelText: l10n.customLabel,
                  border: const OutlineInputBorder(),
                ),
              ),
              if (state.error != null) ...[
                const SizedBox(height: 8),
                Text(state.error!, style: const TextStyle(color: Colors.red)),
              ],
              const SizedBox(height: 20),
              SizedBox(
                width: double.infinity,
                height: 52,
                child: FilledButton(
                  style: FilledButton.styleFrom(
                    backgroundColor: const Color(0xFF2EAE57),
                  ),
                  onPressed: state.saving
                      ? null
                      : () async {
                          final ok = await notifier.saveDraft();
                          if (ok && context.mounted) {
                            Navigator.of(context).pop(true);
                          }
                        },
                  child: state.saving
                      ? const SizedBox(
                          width: 22,
                          height: 22,
                          child: CircularProgressIndicator(
                            strokeWidth: 2,
                            color: Colors.white,
                          ),
                        )
                      : Text(l10n.saveAddress),
                ),
              ),
            ],
          ),
        ),
      ],
    );
  }
}
