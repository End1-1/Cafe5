import 'package:flutter/material.dart';
import 'package:flutter_svg/flutter_svg.dart';

import 'res.dart';

class Styling {
  static const appBarBackgroundColor = Color(0xff009eb3);

  static TextFormField textFormField(
      TextEditingController controller, String hintText,
      {Function(String)? onSubmit,
      int maxLines = 1,
      bool autofocus = false,
      bool readOnly = false,
      GestureTapCallback? onTap,
      Function(String)? onChanged,
      FocusNode? focusNode}) {
    return TextFormField(
      onFieldSubmitted: onSubmit,
      controller: controller,
      maxLines: maxLines,
      readOnly: readOnly,
      autofocus: autofocus,
      onTap: onTap,
      onChanged: onChanged,
      focusNode: focusNode,
      style: const TextStyle(color: Colors.black),
      decoration: InputDecoration(
          contentPadding: const EdgeInsets.fromLTRB(5, 0, 5, 0),
          border: const OutlineInputBorder(
              borderSide: BorderSide(color: Colors.black26)),
          labelText: hintText),
    );
  }

  static TextFormField textFormFieldNumbers(
      TextEditingController controller, String hintText,
      {Function(String)? onSubmit,
      int maxLines = 1,
      bool autofocus = false,
      bool readOnly = false,
      Function(String)? onChange,
      FocusNode? focusNode}) {
    return TextFormField(
      onFieldSubmitted: onSubmit,
      controller: controller,
      maxLines: maxLines,
      keyboardType: TextInputType.number,
      readOnly: readOnly,
      autofocus: autofocus,
      onChanged: onChange,
      focusNode: focusNode,
      style: const TextStyle(color: Colors.black),
      decoration: InputDecoration(
          contentPadding: const EdgeInsets.fromLTRB(5, 15, 5, 0),
          border: const OutlineInputBorder(
              borderSide: BorderSide(color: Colors.black26)),
          labelText: hintText),
    );
  }

  static TextFormField textFormFieldPassword(
      TextEditingController controller, String hintText,
      {ValueChanged<String>? onFieldSubmitted}) {
    return TextFormField(
      obscureText: true,
      controller: controller,
      onFieldSubmitted: onFieldSubmitted,
      style: const TextStyle(color: Colors.black),
      decoration: InputDecoration(
        suffix: IconButton(onPressed: (){}, icon: Icon(Icons.remove_red_eye_outlined)),
          contentPadding: const EdgeInsets.fromLTRB(5, 0, 5, 0),
          border: const OutlineInputBorder(
              borderSide: BorderSide(color: Colors.black26)),
          labelText: hintText),
    );
  }

  static Widget progress() {
    return const SizedBox(
        height: 30, width: 30, child: CircularProgressIndicator());
  }

  static SizedBox columnSpacingWidget() {
    return const SizedBox(height: 10);
  }

  static SizedBox rowSpacingWidget() {
    return const SizedBox(width: 5);
  }

  static TextButton textButton(VoidCallback callback, String text) {
    return TextButton(onPressed: callback, child: Text(text));
  }

  static TextButton textMenuButton(VoidCallback callback, String text) {
    return TextButton(
        onPressed: callback,
        child: Text(text,
            style: const TextStyle(color: Colors.white, fontSize: 16)));
  }

  ///{@template menuButton}
  ///1st: function , 2nd imagename, 3rd - text
  ///{@endtemplate}
  static Widget menuButton(VoidCallback callback, String image, String text) {
    return InkWell(
        onTap: callback,
        child: Row(
            mainAxisAlignment: MainAxisAlignment.start,
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Styling.rowSpacingWidget(),
              SvgPicture.memory(Res.images[image]!,
                  colorFilter: const ColorFilter.mode(
                      Colors.transparent, BlendMode.softLight),
                  height: 25),
              Styling.rowSpacingWidget(),
              Expanded(
                  child: Text(text,
                      maxLines: 2,
                      style:
                          const TextStyle(color: Colors.white, fontSize: 16)))
            ]));
  }

  ///{@template menuButton}
  ///1st: function , 2nd imagename, 3rd - text
  ///{@endtemplate}
  static Widget menuButton2(VoidCallback callback, String image, String text) {
    return InkWell(
        onTap: callback,
        child: Row(
            mainAxisAlignment: MainAxisAlignment.start,
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Styling.rowSpacingWidget(),
              Image.asset('assets/$image.png', height: 25, color: Colors.white),
              Styling.rowSpacingWidget(),
              Expanded(
                  child: Text(text,
                      maxLines: 2,
                      style:
                          const TextStyle(color: Colors.white, fontSize: 16)))
            ]));
  }

  static text(String s, {TextAlign ta = TextAlign.left}) {
    return Text(s, textAlign: ta, style: const TextStyle(color: Colors.black));
  }

  static textBold(String s, {TextAlign ta = TextAlign.left}) {
    return Text(s,
        textAlign: ta,
        style:
            const TextStyle(color: Colors.black, fontWeight: FontWeight.bold));
  }

  static textWithWidth(String s, double w,
      {TextAlign ta = TextAlign.left,
      FontWeight fontWeight = FontWeight.normal}) {
    return SizedBox(
        width: w,
        child: Text(s,
            textAlign: ta,
            style: TextStyle(color: Colors.black, fontWeight: fontWeight)));
  }

  static textError(String s) {
    return Text(s,
        maxLines: 10,
        textAlign: TextAlign.center,
        overflow: TextOverflow.clip,
        style: const TextStyle(color: Colors.red));
  }

  static textCenter(String s) {
    return Text(s,
        maxLines: 10,
        textAlign: TextAlign.center,
        style: const TextStyle(color: Colors.black));
  }
}

class WMCheckbox extends StatefulWidget {
  final String text;
  final Function(bool?) onChange;
  var value = false;

  WMCheckbox(this.text, this.onChange, this.value, {super.key});

  @override
  State<StatefulWidget> createState() => _WMCheckbox();
}

class _WMCheckbox extends State<WMCheckbox> {
  @override
  Widget build(BuildContext context) {
    return Row(children: [
      Checkbox(
          value: widget.value,
          onChanged: (b) {
            setState(() {
              widget.value = b ?? false;
            });
            widget.onChange(b);
          }),
      Styling.text(widget.text)
    ]);
  }
}
