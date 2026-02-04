set path=%PATH%;C:\Development\Qt\6.10.2\msvc2022_64\bin;C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\MSVC\14.50.35717\bin\Hostx64\x64\
lupdate -noobsolete FrontDesk\FrontDeskNetwork.pro -ts FrontDesk\FrontDesk.ts
lupdate -noobsolete Waiter\Waiter.net.pro -ts Waiter\Waiter.ts
lupdate -noobsolete Smart\Smart.net.pro -ts Smart\Smart.ts
lupdate -noobsolete WaiterReports\DailyByWaiter\DailyByWaiter.pro -ts WaiterReports\DailyByWaiter\DailyByWaiter.ts
lupdate -noobsolete WaiterReports\DailyForWash\DailyForWash.pro -ts WaiterReports\DailyForWash\DailyForWash.ts
lupdate -noobsolete WaiterReports\DailyCommon\DailyCommon.pro -ts WaiterReports\DailyCommon\DailyCommon.ts
lupdate -noobsolete WaiterReports\DailyOrders\DailyOrders.pro -ts WaiterReports\DailyOrders\DailyOrders.ts
lupdate -noobsolete WaiterReports\DailyDishes\DailyDishes.pro -ts WaiterReports\DailyDishes\DailyDishes.ts
lupdate -noobsolete WaiterReports\DailyRemovedTillPrecheck\DailyRemovedTillPrecheck.pro -ts WaiterReports\DailyRemovedTillPrecheck\DailyRemovedTillPrecheck.ts
lupdate -noobsolete WaiterReports\DailyRemovedAfterPrecheck\DailyRemovedAfterPrecheck.pro -ts WaiterReports\DailyRemovedAfterPrecheck\DailyRemovedAfterPrecheck.ts
lupdate -noobsolete Shop\Shop.net.pro -ts Shop\Shop.ts
lupdate -noobsolete Server5\Server5.pro -ts Server5\Server5.ts
lupdate -noobsolete Service5\Service5.pro -ts Service5\Service5.ts
pause