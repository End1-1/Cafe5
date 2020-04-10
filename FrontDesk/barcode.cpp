#include "barcode.h"

Barcode::Barcode()
{

}

QString Barcode::Code_128(const QString &A) const {
    int BCode[1023] = {0};
    int BInd = 0;
    QString CurMode;
    int I = 0;
    int LenA = A.length() - 1;
    while (I <= LenA) {
        int Ch = A.mid(I, 1).at(0).toLatin1();
        int Ch2;
        I++;
        if (Ch <= 127) {
            if (I <= LenA) {
                Ch2 = A.mid(I, 1).at(0).toLatin1();
            } else {
                Ch2 = 0;
            }
            if ((48 <= Ch) && (Ch <= 57) && (48 <= Ch2) && (Ch2 <= 57)) {
                I = I + 1;
                if (BInd == 0) {
                    CurMode = "C";
                    BCode[BInd] = 105;
                    BInd = BInd + 1;
                } else if (CurMode != "C") {
                    CurMode = "C";
                    BCode[BInd] = 99;
                    BInd = BInd + 1;
                }
                BCode[BInd] = ((QString(QChar(Ch)) + QString(QChar(Ch2))).toInt());
                BInd = BInd + 1;
            } else {
                if (BInd == 0) {
                    if (Ch < 32) {
                        CurMode = "A";
                        BCode[BInd] = 103;
                        BInd = BInd + 1;
                    } else {
                        CurMode = "B";
                        BCode[BInd] = 104;
                        BInd = BInd + 1;
                    }
                }
                if ((Ch < 32) && (CurMode != "A")) {
                    CurMode = "A";
                    BCode[BInd] = 101;
                    BInd = BInd + 1;
                } else if (((64 <= Ch) && (CurMode != "B")) || (CurMode == "C")) {
                    CurMode = "B";
                    BCode[BInd] = 100;
                    BInd = BInd + 1;
                }
                if (Ch < 32) {
                    BCode[BInd] = Ch + 64;
                    BInd = BInd + 1;
                } else {
                    BCode[BInd] = Ch - 32;
                    BInd = BInd + 1;
                }
            }
        }
    }
    int CCode = BCode[0] % 103;
    for (int i = 1; i < BInd; i++) {
        CCode = (CCode + BCode[i] * i) % 103;
    }
    BCode[BInd] = CCode;
    BInd++;
    BCode[BInd] = 106;
    BInd++;

    QString S;
    for (I = 0; I < BInd; I++) {
        S += Code_Char(Code_128_ID(BCode[I]));
    }

    return S;
}

QString Barcode::Code_Interlived_2_5(const QString &A, bool Check) const
{
        int I;
        int Ch;
        int K;
        QString D;
        for (I = 0; I < A.length(); I++) {
            Ch = A.at(I).toLatin1();
            if ((48 <= Ch) && (Ch <= 57)) {
                D += QChar(Ch);
            }
        }

        if (((D.length() % 2 > 0) && (!Check)) || ((D.length() % 2 == 0) && Check)) {
            D = "0" + D;
        }

        if (Check) {
            K = 0;
            for (I = 0; I < D.length(); I++) {
                if (I % 2 > 0) {
                    K = K + D.mid(I, 1).at(0).toLatin1() * 3;
                } else {
                    K = K + D.mid(I, 1).at(0).toLatin1();
                }
            }
            K = 10 - (K % 10);
            D += QString::number(K);
        }

        QString S;
        for (I = 0; I < D.length() / 2; I++) {
            QString mid = D.mid(I * 2, 2);
            QString i2of5 = Interleaved_2of5_Pair(mid);
            S += Code_Char(i2of5);
        }

        return Code_Char("1111") + S + Code_Char("3111");
}


QString Barcode::Code_Char(const QString &A) const {
    QString S;
    if (A == "211412") {
        S = "A";
    } else if (A == "211214") {
        S = "B";
    } else if (A == "211232") {
        S = "C";
    } else if (A == "2331112") {
        S = "@";
    } else {
        for (int I = 0; I < A.length() / 2; I++) {
            QString B = A.mid(2 * I, 2);
            if      (B == "11") {S += "0";}
            else if (B =="21") {S += "1";}
            else if (B =="31") {S += "2";}
            else if (B =="41") {S += "3";}
            else if (B =="12") {S += "4";}
            else if (B =="22") {S += "5";}
            else if (B =="32") {S += "6";}
            else if (B =="42") {S += "7";}
            else if (B =="13") {S += "8";}
            else if (B =="23") {S += "9";}
            else if (B =="33") {S += ":";}
            else if (B =="43") {S += ";";}
            else if (B =="14") {S += "<";}
            else if (B =="24") {S += "=";}
            else if (B =="34") {S += ">";}
            else if (B =="44") {S += "?";}
        }
    }
    return S;
}

QString Barcode::Code_128_ID(int ID) const {
    switch (ID) {
        case 0:return "212222";
        case 1:return "222122";
        case 2:return "222221";
        case 3:return "121223";
        case 4:return "121322";
        case 5:return "131222";
        case 6:return "122213";
        case 7:return "122312";
        case 8:return "132212";
        case 9:return "221213";
        case 10:return "221312";
        case 11:return "231212";
        case 12:return "112232";
        case 13:return "122132";
        case 14:return "122231";
        case 15:return "113222";
        case 16:return "123122";
        case 17:return "123221";
        case 18:return "223211";
        case 19:return "221132";
        case 20:return "221231";
        case 21:return "213212";
        case 22:return "223112";
        case 23:return "312131";
        case 24:return "311222";
        case 25:return "321122";
        case 26:return "321221";
        case 27:return "312212";
        case 28:return "322112";
        case 29:return "322211";
        case 30:return "212123";
        case 31:return "212321";
        case 32:return "232121";
        case 33:return "111323";
        case 34:return "131123";
        case 35:return "131321";
        case 36:return "112313";
        case 37:return "132113";
        case 38:return "132311";
        case 39:return "211313";
        case 40:return "231113";
        case 41:return "231311";
        case 42:return "112133";
        case 43:return "112331";
        case 44:return "132131";
        case 45:return "113123";
        case 46:return "113321";
        case 47:return "133121";
        case 48:return "313121";
        case 49:return "211331";
        case 50:return "231131";
        case 51:return "213113";
        case 52:return "213311";
        case 53:return "213131";
        case 54:return "311123";
        case 55:return "311321";
        case 56:return "331121";
        case 57:return "312113";
        case 58:return "312311";
        case 59:return "332111";
        case 60:return "314111";
        case 61:return "221411";
        case 62:return "431111";
        case 63:return "111224";
        case 64:return "111422";
        case 65:return "121124";
        case 66:return "121421";
        case 67:return "141122";
        case 68:return "141221";
        case 69:return "112214";
        case 70:return "112412";
        case 71:return "122114";
        case 72:return "122411";
        case 73:return "142112";
        case 74:return "142211";
        case 75:return "241211";
        case 76:return "221114";
        case 77:return "413111";
        case 78:return "241112";
        case 79:return "134111";
        case 80:return "111242";
        case 81:return "121142";
        case 82:return "121241";
        case 83:return "114212";
        case 84:return "124112";
        case 85:return "124211";
        case 86:return "411212";
        case 87:return "421112";
        case 88:return "421211";
        case 89:return "212141";
        case 90:return "214121";
        case 91:return "412121";
        case 92:return "111143";
        case 93:return "111341";
        case 94:return "131141";
        case 95:return "114113";
        case 96:return "114311";
        case 97:return "411113";
        case 98:return "411311";
        case 99:return "113141";
        case 100:return "114131";
        case 101:return "311141";
        case 102:return "411131";
        case 103:return "211412";
        case 104:return "211214";
        case 105:return "211232";
        case 106:return "2331112";
    }
    return "";
}

QString Barcode::Interleaved_2of5_Pair(const QString &Pair) const
{
    QString S1 = Code_2of5_Ch(Pair.mid(0, 1));
    QString S2 = Code_2of5_Ch(Pair.mid(1, 1));
    QString S;
    for (int i = 0; i < S1.length(); i++) {
        S +=  S1.mid(i, 1) + S2.mid(i, 1);
    }
    return S;
}

QString Barcode::Code_2of5_Ch(const QString &Ch) const
{
    if (Ch == "0") {
        return "11331";
    } else if (Ch == "1") {
        return "31113";
    } else if (Ch == "2") {
        return "13113";
    } else if (Ch == "3") {
        return "33111";
    } else if (Ch == "4") {
        return "11313";
    } else if (Ch == "5") {
        return "31311";
    } else if (Ch == "6") {
        return "13311";
    } else if (Ch == "7") {
        return "11133";
    } else if (Ch == "8") {
        return "31131";
    } else if (Ch == "9") {
        return "13131";
    }
    return "???";
}
