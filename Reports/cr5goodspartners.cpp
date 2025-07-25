#include "cr5goodspartners.h"
#include "ce5partner.h"
#include "partnersasmap.h"
#include "c5mainwindow.h"

CR5GoodsPartners::CR5GoodsPartners(QWidget *parent) :
    C5ReportWidget( parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Partners");
    fSqlQuery = "select p.f_id, c.f_name as f_category, s.f_name as f_state, g.f_name as f_group, "
                "p.f_name, p.f_taxname, p.f_taxcode, p.f_contact, p.f_phone, p.f_email, "
                "p.f_address, p.f_legaladdress, p.f_info, st.f_name as f_saletype, p.f_permanent_discount as f_discount, "
                "concat_ws(' ', u.f_last, u.f_first) as f_manager "
                "from c_partners p "
                "left join c_partners_category c on c.f_id=p.f_category "
                "left join c_partners_state s on s.f_id=p.f_state "
                "left join c_partners_group g on g.f_id=p.f_group "
                "left join o_sale_type st on st.f_id=p.f_price_politic "
                "left join s_user u on u.f_id=p.f_manager ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_category"] = tr("Category");
    fTranslation["f_state"] = tr("State");
    fTranslation["f_group"] = tr("Group");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_taxname"] = tr("Legal name");
    fTranslation["f_taxcode"] = tr("Taxpayer id");
    fTranslation["f_contact"] = tr("Contact");
    fTranslation["f_info"] = tr("Info");
    fTranslation["f_phone"] = tr("Phone");
    fTranslation["f_email"] = tr("Email");
    fTranslation["f_address"] = tr("Address");
    fTranslation["f_legaladdress"] = tr("Legal address");
    fTranslation["f_discount"] = tr("Discount");
    fTranslation["f_saletype"] = tr("Sale type");
    fTranslation["f_manager"] = tr("Manager");
    fEditor = new CE5Partner();
}

QToolBar *CR5GoodsPartners::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        createStandartToolbar(btn);
        fToolBar->addAction(QIcon(":/AS.png"), tr("ArmSoft map"), this, SLOT(armSoftMap()));
    }
    return fToolBar;
}

void CR5GoodsPartners::armSoftMap()
{
    __mainWindow->createTab<PartnersAsMap>();
}
