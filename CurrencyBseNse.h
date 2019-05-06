#ifndef CURRENCYBSENSE_H
#define CURRENCYBSENSE_H
#include <sgContext.h>
#include <api2UserCommands.h>
#include "../common/orderWrapper.h"
#include <map>

class CurrencyBseNse: public API2::SGContext
{
    std::map<API2::COMMON::OrderId*,API2_NEW::COMMON::OrderWrapper*> OrderIdOrderWrapperBuyMap;
    std::map<API2::COMMON::OrderId*,API2_NEW::COMMON::OrderWrapper*> OrderIdOrderWrapperSellMap;
    std::map<API2::COMMON::OrderId*,API2_NEW::COMMON::OrderWrapper*> OrderIdOrderWrapperBuyMapSelfTrade;
    std::map<API2::COMMON::OrderId*,API2_NEW::COMMON::OrderWrapper*> OrderIdOrderWrapperSellMapSelfTrade;

    struct timespec start, end;
    API2::DATA_TYPES::String _IsBidding,IsBidding;
    API2::DATA_TYPES::RiskStatus risk;
    API2::AccountDetail tempAccount;
    API2::DATA_TYPES::String Symbol1, Symbol2, dd, TotalBuyTraded, TotalSellTraded;
    API2::DATA_TYPES::SYMBOL_ID symbolId1, symbolId2;   
    UNSIGNED_SHORT _OrderType, _OrderValidity, _OrderProduct, _BuySide, _SellSide;
    API2::AccountDetail accountDetailLeg1, accountDetailLeg2, AccountDetailNse, AccountDetailBse;

    API2::COMMON::Instrument *_tradeInstrument1, *_tradeInstrument2, *_NseTradeInstr, *_BseTradeInstr, *mktDataNse, *_FeedInstrumentNse;
    API2_NEW::COMMON::OrderWrapper _orderWrapperBuy, _orderWrapperSell, _orderWrapperSecondBuy, _orderWrapperSecondSell;
    char Reset_All_Qty, Depth, _Depth;

    API2::DATA_TYPES::PRICE _BuyDiff, _SellDiff, BuyPriceNse, SellPriceNse, BuyQuotePrice, SellQuotePrice, BuyDiff1, SellDiff1;
    API2::DATA_TYPES::PRICE First_Leg_BuyOrder_Price, First_Leg_SellOrder_Price, TrdPrice, _TriggerPrice, TriggerPrice1, dTrigger;
    API2::DATA_TYPES::PRICE _StopLoss, StopLoss1,SecLegBuyOrderP, SecLegSellOrderP, _Tolerance, Tolerance1, dTradeDiff;
    API2::DATA_TYPES::PRICE First_Sell_Traded_Price, Second_Buy_Traded_Price, First_Buy_Traded_Price, Second_Sell_Traded_Price;
    API2::DATA_TYPES::PRICE BuyPriceNse1, SellPriceNse1,  OrderPriceBuySecond, OrderPriceSellSecond;
    API2::DATA_TYPES::PRICE FirstLegBuyTradePrice, FirstLegSellTradePrice, OrderPrice, NewPrice;

    API2::DATA_TYPES::QTY Maturity1, Maturity2, MaxOrderLot, _MaxOrderLot, BuyTotalLot, _BuyTotalLot, SellTotalLot, _SellTotalLot;
    API2::DATA_TYPES::QTY First_Buy_Pending_Qty, First_Sell_Pending_Qty, Second_Buy_Pending_Qty,  Second_Sell_Pending_Qty, TrdQty;
    API2::DATA_TYPES::QTY First_leg_BuyTraded,First_leg_SellTraded, Second_leg_BuyTraded, Second_leg_SellTraded, MaturityDay1, MaturityDay2;
    API2::DATA_TYPES::QTY BuyTradedQty, SellTradedQty, SecondBuyTradeQty, SecondSellTradeQty, SecLegSellOrderQ, SecLegBuyOrderQ;
    API2::DATA_TYPES::QTY BuySizeSize, BuySizeSize1, SellSizeSize,SellSizeSize1, TickSize, BoardLotSizeNse, BoardLotSizeBse, Error, ErrorFlag, _TotalSellTraded;
    API2::DATA_TYPES::QTY OrderBuySize, OrderSellSize, MinSize, First_Buy_Traded_Qty, First_Sell_Traded_Qty, Second_Sell_Traded_Qty;
    API2::DATA_TYPES::QTY Second_Buy_Traded_Qty, _TotalBuyTraded, First_leg_Buy_Position, First_leg_Sell_Position, cond, iCond;
    API2::DATA_TYPES::QTY Second_leg_Buy_Position, Second_leg_Sell_Position, FirstLegBuyTradedQty, FirstLegSellTradedQty, DateDiff;
    API2::DATA_TYPES::QTY SecondLegBuyPlacedQty, SecondLegSellPlacedQty, FeedCount, RemBuyQty, RemSellQty, ParamError, FlushCounter;    
    API2::DATA_TYPES::QTY pBuyPriceNse, pBuyPriceNse1, pSellPriceNse, pSellPriceNse1, prevCount;
    API2::DATA_TYPES::QTY pBuySizeSize, pBuySizeSize1, pSellSizeSize, pSellSizeSize1;
    // new change
    CREATE_FIELD(SIGNED_LONG , AlgoId);

public:

    CurrencyBseNse(API2::StrategyParameters *params);
    ~CurrencyBseNse();
    void onCMDModifyStrategy(API2::AbstractUserParams *newParams);
    void onFilled(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId);
    void onReplaced(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId);
    void onCanceled(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId);
    void onConfirmed(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId);
    void onPartialFill(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId);
    void onMarketDataEvent(UNSIGNED_LONG symbolId);
    bool setParameters(API2::UserParams *params);
    static void TestDriveStrategy(void *params);
    void onReplaceRejected(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId);
    void onCancelRejected(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId);
    void onNewReject(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId);
    // void onProcessOrderConfirmation(API2::OrderConfirmation &confirmation);
    int FindMinimumSize(API2::DATA_TYPES::QTY Size1,API2::DATA_TYPES::QTY Size2);
    void Update_Text(API2::DATA_TYPES::QTY qty);
    void Update_Status(API2::DATA_TYPES::String str1);
    void Update_Buy_Position(API2::DATA_TYPES::QTY qty);
    void Update_Sell_Position(API2::DATA_TYPES::QTY qty);	
    //void onTimerEvent();
};
#endif // CurrencyBseNse_H
