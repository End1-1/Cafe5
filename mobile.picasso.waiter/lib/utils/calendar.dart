import 'package:picassowaiter/utils/prefs.dart';
import 'package:flutter/material.dart';

class Calendar {
  static Future<DateTime?> show(
      {DateTime? firstDate, DateTime? currentDate, DateTime? lastDate}) {
    return showDatePicker(
        context: prefs.context(),
        initialEntryMode: DatePickerEntryMode.calendarOnly,
        currentDate: currentDate ?? prefs.workingDay(),
        firstDate: firstDate ?? prefs.workingDay(),
        lastDate: lastDate ?? DateTime.now().add(const Duration(days: 30 * 120)));
  }
}
