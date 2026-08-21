// ignore: unused_import
import 'package:intl/intl.dart' as intl;
import 'app_localizations.dart';

// ignore_for_file: type=lint

/// The translations for English (`en`).
class AppLocalizationsEn extends AppLocalizations {
  AppLocalizationsEn([String locale = 'en']) : super(locale);

  @override
  String get appName => 'ArariX Solutions';

  @override
  String get welcome => 'Welcome';

  @override
  String get registerWithPhone => 'Register with your phone number';

  @override
  String get prefix => 'Prefix';

  @override
  String get phoneNumber => 'Phone number';

  @override
  String get sms => 'SMS';

  @override
  String get whatsapp => 'WHATSAPP';

  @override
  String get orWith => 'or with';

  @override
  String get google => 'Google';

  @override
  String get facebook => 'Facebook';

  @override
  String get termsFooter =>
      'By continuing, you automatically accept our Terms and conditions, Privacy Policy and Cookies policy';

  @override
  String get selectCountry => 'Select country';

  @override
  String get searchCountry => 'Search for a country';

  @override
  String get armenia => 'Armenia';

  @override
  String get phoneValidation => 'Phone number validation';

  @override
  String get enterTheCode => 'Enter the code';

  @override
  String insertCodeWhatsapp(String phone) {
    return 'Insert the 4-digit code that we sent via WhatsApp to $phone';
  }

  @override
  String insertCodeSms(String phone) {
    return 'Insert the 4-digit code that we sent via SMS to $phone';
  }

  @override
  String resendIn(String time) {
    return 'Still nothing? Resend the code in $time';
  }

  @override
  String get resendNow => 'Resend the code';

  @override
  String get account => 'Account';

  @override
  String get enterYourName => 'Enter Your name';

  @override
  String get enterYourEmail => 'Enter Your email';

  @override
  String get changePassword => 'Change or enter your password';

  @override
  String get addAddresses => 'Add your addresses';

  @override
  String get managePayments => 'Manage payment options';

  @override
  String get profile => 'Profile';

  @override
  String get orderHistory => 'Order history';

  @override
  String get paymentMethods => 'Payment methods';

  @override
  String get language => 'Language';

  @override
  String get faq => 'FAQ';

  @override
  String get notifications => 'Notifications';

  @override
  String get deleteAccount => 'Delete my account and data';

  @override
  String get silverStatus => 'Silver status';

  @override
  String get changePhoneNumber => 'Change phone number';

  @override
  String get enterYourPhoneNumber => 'Enter your phone number';

  @override
  String get payments => 'Payments';

  @override
  String get addNewCard => 'Add new card';

  @override
  String get helpAndSupport => 'Help and support';

  @override
  String get chooseLanguage => 'Choose your language';

  @override
  String get home => 'Home';

  @override
  String get menu => 'Menu';

  @override
  String get orders => 'Orders';

  @override
  String get comingSoon => 'Coming soon';

  @override
  String get socialLoginSoon => 'Social login will be available later';

  @override
  String get invalidPhone => 'Enter a valid phone number';

  @override
  String get invalidOtp => 'Enter the 4-digit code';

  @override
  String get errorGeneric => 'Something went wrong';

  @override
  String get save => 'Save';

  @override
  String get name => 'Name';

  @override
  String get email => 'Email';

  @override
  String get english => 'English';

  @override
  String get russian => 'Russian';

  @override
  String get armenian => 'Armenian';

  @override
  String get englishNative => 'English';

  @override
  String get russianNative => 'Русский';

  @override
  String get armenianNative => 'Հայերեն';

  @override
  String get searchRestaurant => 'Search for a restaurant';

  @override
  String get delivery => 'Delivery';

  @override
  String get takeaway => 'Takeaway';

  @override
  String get dineIn => 'Dine-In';

  @override
  String get topRestaurants => 'Top Restaurants';

  @override
  String get dishes => 'Dishes';

  @override
  String get topOffer => 'Top offer';

  @override
  String get retry => 'Retry';
}
