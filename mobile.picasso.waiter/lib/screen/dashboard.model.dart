part of 'dashboard.dart';

class DashboardModel {
  final checkin = [];
  final inhouse = [];
  final checkout = [];
  final drafts = [];

  var date1 = DateTime.now();
  var date2 = DateTime.now();
  final date1Controller = TextEditingController();
  final date2Controller = TextEditingController();

  var hallFilter = 0;
  final halls = <dynamic>[];
  var hallId = 0;
  var hallName = '';
  final tables = <dynamic>[];
  final openTables = {};
  final filteredTables = {};

  DashboardModel() {
    state();
  }

  void state() {
    date1Controller.text = prefs.dateText(date1);
    date2Controller.text = prefs.dateText(date2);
  }

  void changeDate1() {
    Calendar.show(
            firstDate: date1.add(const Duration(days: -365 * 3)),
            currentDate: date1)
        .then((value) {
      if (value != null) {
        date1 = value;
        state();
      }
    });
  }

  void changeDate2() {
    Calendar.show(
            firstDate: date1.add(const Duration(days: -365 * 3)),
            currentDate: date2)
        .then((value) {
      if (value != null) {
        date2 = value;
        state();
      }
    });
  }
}

class AppStateDashboard extends AppStateFinished {
  AppStateDashboard({required super.data});
}

extension WMEDashboard on WMDashboard {
  void getDashboard() {
    getDashboardWaiter();
  }



  void removeDraft(String id) {
    BlocProvider.of<QuestionBloc>(prefs.context())
        .add(QuestionEventRaise(model.tr('Confirm to remove draft'), () {
      BlocProvider.of<AppBloc>(prefs.context()).add(AppEventLoading(
          model.tr('Wait, please'), 'engine/shop/remove-draft.php', {'id': id},
          (e, d) {
        if (e) {
          return;
        }

        getDashboard();
      }, AppStateDashboard(data: _model)));
    }, () {}));
  }
}
