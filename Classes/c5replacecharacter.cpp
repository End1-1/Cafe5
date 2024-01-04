#include "c5replacecharacter.h"

C5ReplaceCharacter::C5ReplaceCharacter()
{

}

QString C5ReplaceCharacter::replace(const QString &str)
{
    QString hya("էթփձջւևրչճԷԹՓՁՋՒևՐՉՃ"
                "քոեռտըւիօպխծ"
                "ասդֆգհյկլ;՛շ"
                "զղցվբնմ,․/"
                "ՔՈԵՌՏԸՒԻՕՊԽԾ"
                "ԱՍԴՖԳՀՅԿԼ;՛Շ"
                "ԶՂՑՎԲՆՄ,․/");
    QString rus("12345678901234567890"
                "йцукенгшщзхъ"
                "фывапролджэ\\"
                "ячсмитьбю."
                "ЙЦУКЕНГШЩЗХЪ"
                "ФЫВАПРОЛДЖЭ\\"
                "ЯЧСМИТЬБЮ.");
    QString num("12345678901234567890"
                "QWERTYUIOP[]"
                "ASDFGHJKL;'\""
                "ZXCVBNM,./"
                "QWERTYUIOP[]"
                "ASDFGHJKL;'\""
                "ZXCVBNM,./");
    QString out;
    for (int i = 0; i < str.length(); i++) {
        int p = hya.indexOf(str.at(i));
        if (p != -1) {
            out += num.at(p);
            continue;
        }
        p = rus.indexOf(str.at(i));
        if (p != -1) {
            out += num.at(p);
            continue;
        }
        out += str.at(i);
    }
    return out.toUpper();
}
