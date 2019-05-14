// 20 Feb 2018.
// 6000
#include "CurrencyBseNse.h"
#include <api2Exceptions.h>
#include <time.h>

CurrencyBseNse::CurrencyBseNse(API2::StrategyParameters *params):API2::SGContext(params,"NSEvsBSECurrencyUtrade1.0MTBT"), _tradeInstrument1(NULL), _tradeInstrument2(NULL)
{
    // DEBUG_MESSAGE(reqQryDebugLog(), "------------- Inside constructor");
    API2::UserParams *frontendParams = (API2::UserParams *)params->getInfo();
    frontendParams->dump();   

	try{
        if(!setParameters(frontendParams))
		{
            reqAddStrategyComment(API2::CONSTANTS::RSP_StrategyComment_STRATEGY_ERROR_STATE);
            DEBUG_FLUSH(reqQryDebugLog());
            reqTerminateStrategy();
		}
    }
	catch(API2::MarketDataSubscriptionFailedException &e)
	{
        reqAddStrategyComment(API2::CONSTANTS::RSP_StrategyComment_STRATEGY_ERROR_STATE);
        DEBUG_FLUSH(reqQryDebugLog());
        reqTerminateStrategy();
        return;
	}

    catch(API2::InstrumentNotFoundException &e)
    {
        DEBUG_MESSAGE(reqQryDebugLog(),"Instrument Not Found");
        reqAddStrategyComment(API2::CONSTANTS::RSP_StrategyComment_STRATEGY_ERROR_STATE);
        DEBUG_FLUSH(reqQryDebugLog());
        reqTerminateStrategy();
        return;
    }
    catch(std::exception &e)
    {
        DEBUG_MESSAGE(reqQryDebugLog(),"standard exception raised");
        reqAddStrategyComment(API2::CONSTANTS::RSP_StrategyComment_STRATEGY_ERROR_STATE);
        DEBUG_FLUSH(reqQryDebugLog());
        reqTerminateStrategy();
        return;
    }

    if(ParamError == 1)
    {       
        reqAddStrategyComment(API2::CONSTANTS::RSP_StrategyComment_STRATEGY_ERROR_STATE);
        DEBUG_FLUSH(reqQryDebugLog());
        reqTerminateStrategy();
        return;
    }


    if(_NseTradeInstr == NULL || _BseTradeInstr == NULL)
    {
        DEBUG_MESSAGE(reqQryDebugLog(),"_NseTradeInstr == NULL || _BseTradeInstr == NULL");
        reqAddStrategyComment(API2::CONSTANTS::RSP_StrategyComment_STRATEGY_ERROR_STATE);
        DEBUG_FLUSH(reqQryDebugLog());
        reqTerminateStrategy();
        return;
    }
    else
    {
        _algoIdSet.insert( _NseTradeInstr->getStaticData()->marketId);
        _algoIdSet.insert( _BseTradeInstr->getStaticData()->marketId);
        readConfForAlgoid("SAMPLE-ALGO","NseBseSample.conf");               // create file in config folder
    }

    if( !validateAlgoDetail() )
    {
        DEBUG_MESSAGE(reqQryDebugLog(),"Invalid Algo Id " );
        reqAddStrategyComment(API2::CONSTANTS::RSP_StrategyComment_STRATEGY_ERROR_STATE);
        DEBUG_FLUSH(reqQryDebugLog());
        reqTerminateStrategy();
        return ;
    }

   // reqTimerEvent(5000000);     // 5 seconds
}

CurrencyBseNse::~CurrencyBseNse()
{
}

int CurrencyBseNse::FindMinimumSize(API2::DATA_TYPES::QTY Size1,API2::DATA_TYPES::QTY Size2)
{
    if(Size1 > Size2){
        MinSize = Size2;
    }
    else{
        MinSize = Size1;
    }

    if(MinSize <= 1){
        MinSize = 1;
    }

    return MinSize;
} 	// end of FindMinimumSize();


void CurrencyBseNse::Update_Buy_Position(API2::DATA_TYPES::QTY Qty)
{
    std::ostringstream str1,str2;
    std::list<std::string> list1;
    std::string z;
    int _rowId = 0;
    str2 << Qty;
    str1 << "TotalBuyTraded:" << str2.str();

    list1.push_back(str1.str());
    if(!reqQrySendCustomResponse(z,list1,_rowId))
    {
        // DEBUG_MESSAGE(reqQryDebugLog(),"CustomResponseFailed");
        DEBUG_FLUSH(reqQryDebugLog());
        reqTerminateStrategy(API2::CONSTANTS::RSP_StrategyComment_STRATEGY_ERROR_STATE);
        return;
    }
}

void CurrencyBseNse::Update_Sell_Position(API2::DATA_TYPES::QTY Qty)
{
    std::ostringstream str1, str2;
    std::list<std::string> list1;
    std::string z;
    int _rowId = 0;
    str2 << Qty;
    str1 << "TotalSellTraded:" << str2.str();

    list1.push_back(str1.str());
    if(!reqQrySendCustomResponse(z,list1,_rowId))
    {
        DEBUG_MESSAGE(reqQryDebugLog(),"CustomResponseFailed");
        DEBUG_FLUSH(reqQryDebugLog());
        reqTerminateStrategy(API2::CONSTANTS::RSP_StrategyComment_STRATEGY_ERROR_STATE);
        return;
    }
}

void CurrencyBseNse::Update_Text(API2::DATA_TYPES::QTY Q1)
{
    std::ostringstream str1;
    std::list<std::string> list1;
    std::string z;
    int _rowId = 0;

    str1 << "IsBidding:" << "No";
    list1.push_back(str1.str());
    if(!reqQrySendCustomResponse(z,list1,_rowId))
    {
        DEBUG_MESSAGE(reqQryDebugLog(),"CustomResponseFailed");
        DEBUG_FLUSH(reqQryDebugLog());
        reqTerminateStrategy(API2::CONSTANTS::RSP_StrategyComment_STRATEGY_ERROR_STATE);
        return;
    }
}

void CurrencyBseNse::Update_Status(API2::DATA_TYPES::String Str11)
{
    std::ostringstream str1;
    std::list<std::string> list1;
    std::string z;
    int _rowId = 0;

    str1 << "IsBidding:" << Str11;
    list1.push_back(str1.str());

    if(!reqQrySendCustomResponse(z,list1,_rowId))
    {
        DEBUG_MESSAGE(reqQryDebugLog(),"CustomResponseFailed");
        DEBUG_FLUSH(reqQryDebugLog());
        reqTerminateStrategy(API2::CONSTANTS::RSP_StrategyComment_STRATEGY_ERROR_STATE);
        return;
    }
}

void CurrencyBseNse::onCMDModifyStrategy(API2::AbstractUserParams *newParams)
{
    DEBUG_MESSAGE(reqQryDebugLog(), "------------Inside onCMDModifyStrategy");
    API2::UserParams *params =(API2::UserParams *) newParams;	

    if(params->getValue("BuyTotalLot",BuyTotalLot) != API2::UserParamsError_OK || params->getValue("SellTotalLot",SellTotalLot) != API2::UserParamsError_OK)
    {
        DEBUG_MESSAGE(reqQryDebugLog(), "issue in BuyTotalLot OR SellTotalLot");
        ErrorFlag = 1;
        Update_Status("issue in BuyTotalLot OR SellTotalLot");
        DEBUG_FLUSH(reqQryDebugLog());
        reqTerminateStrategy();
    }
    else
    {
        if(BuyTotalLot - _TotalBuyTraded > 1000 || SellTotalLot - _TotalSellTraded > 1000)
        {
            DEBUG_MESSAGE(reqQryDebugLog(), "Open BuyTotal_Lot OR SellTotal_Lot cant be more than 1000");
            ErrorFlag = 1;
            Update_Status("issue in BuyTotalLot - _TotalBuyTraded OR SellTotalLot - _TotalSellTraded");
            DEBUG_FLUSH(reqQryDebugLog());
            reqTerminateStrategy();
        }
        else
        {
            _BuyTotalLot = BuyTotalLot;
            _SellTotalLot = SellTotalLot;
        }
    }

    First_Buy_Traded_Qty = _BseTradeInstr -> getPosition()-> getTradedQty(API2::CONSTANTS::CMD_OrderMode_BUY)/BoardLotSizeBse;
    First_Sell_Traded_Qty = _BseTradeInstr -> getPosition()-> getTradedQty(API2::CONSTANTS::CMD_OrderMode_SELL)/BoardLotSizeBse;

    if(_BuyTotalLot - First_Buy_Traded_Qty > 1000 || _SellTotalLot - First_Sell_Traded_Qty > 1000)
    {
        DEBUG_MESSAGE(reqQryDebugLog(), "issue in _BuyTotalLot / _SellTotalLot excess Quantity");
        ErrorFlag = 1;
        Update_Status("issue in _BuyTotalLot / _SellTotalLot excess Quantity");
        DEBUG_FLUSH(reqQryDebugLog());
        reqTerminateStrategy();
    }

    
    if(params->getValue("MaxOrderLot",MaxOrderLot) != API2::UserParamsError_OK)
    {
        DEBUG_MESSAGE(reqQryDebugLog(), "issue in MaxOrderLot");
        ErrorFlag = 1;
        Update_Status("issue in MaxOrderLot");
        DEBUG_FLUSH(reqQryDebugLog());
        reqTerminateStrategy();
    }
    else
    {
        if(MaxOrderLot > 0 && MaxOrderLot < 50)
        {
            _MaxOrderLot = MaxOrderLot;
        }
        else
        {
            DEBUG_MESSAGE(reqQryDebugLog(), "issue in MaxOrderLot");
            ErrorFlag = 1;
            Update_Status("issue in MaxOrderLot");
            DEBUG_FLUSH(reqQryDebugLog());
            reqTerminateStrategy();
        }
    }

    if(params->getValue("BuyDiff",BuyDiff1) != API2::UserParamsError_OK || params->getValue("SellDiff",SellDiff1) != API2::UserParamsError_OK)
    {
        DEBUG_MESSAGE(reqQryDebugLog(), "issue in Difference entered into user Parameters.");
        ErrorFlag = 1;
        Update_Status("issue in Difference entered into user Parameters");
        DEBUG_FLUSH(reqQryDebugLog());
        reqTerminateStrategy();
    }
    else
    {
        if(BuyDiff1 + SellDiff1 < 0)
        {
            DEBUG_MESSAGE(reqQryDebugLog(), "Sum of Buy/Sell Difference can't be less than Zero");
            ErrorFlag = 1;
            Update_Status("Sum of Buy/Sell Difference can't be less than Zero");
            DEBUG_FLUSH(reqQryDebugLog());
            reqTerminateStrategy();
        }
        else
        {
            if(BuyDiff1 < -100 || SellDiff1 < -100)
            {
                DEBUG_MESSAGE(reqQryDebugLog(), "Sum of BuyDiff1 < -0.0100 || SellDiff1 < 0.0100 ");
                ErrorFlag = 1;
                Update_Status("Sum of BuyDiff1 < -0.0100 || SellDiff1 < 0.0100 ");
                DEBUG_FLUSH(reqQryDebugLog());
                reqTerminateStrategy();
            }
            else
            {
                _BuyDiff = BuyDiff1;
                _SellDiff = SellDiff1;
            }
        }
    }

    if(params->getValue("IsBidding",IsBidding) != API2::UserParamsError_OK)
    {
        DEBUG_MESSAGE(reqQryDebugLog(), "issue in IsBidding");
        ErrorFlag = 1;
        Update_Status("issue in IsBidding");
        DEBUG_FLUSH(reqQryDebugLog());
        reqTerminateStrategy();
    }
    else{
        _IsBidding = IsBidding;
    }

    if(params->getValue("StopLoss",StopLoss1) != API2::UserParamsError_OK || params->getValue("TriggerPrice",TriggerPrice1) != API2::UserParamsError_OK || params->getValue("Tolerance",Tolerance1) != API2::UserParamsError_OK)
    {
        DEBUG_MESSAGE(reqQryDebugLog(), "issue in Stoploss / TriggerPrice / Tolerance  entered into user Parameters.");
        ErrorFlag = 1;
        Update_Status("issue in Stoploss / TriggerPrice / Tolerance  entered into user Parameters.");
        DEBUG_FLUSH(reqQryDebugLog());
        reqTerminateStrategy();
    }
    else
    {
        _StopLoss = StopLoss1;
        _TriggerPrice = TriggerPrice1;
        _Tolerance = Tolerance1;
    }   

    if(params->getValue("Depth",Depth) != API2::UserParamsError_OK)
    {
        DEBUG_MESSAGE(reqQryDebugLog(), "issue in Depth");
        ErrorFlag = 1;
        Update_Status("issue in Depth");
        DEBUG_FLUSH(reqQryDebugLog());
        reqTerminateStrategy();
    }
    else{
        _Depth = Depth;
    }

    reqSendStrategyResponse(API2::CONSTANTS::RSP_ResponseType_STRATEGY_RUNNING,API2::CONSTANTS::RSP_RiskStatus_SUCCESS,API2::CONSTANTS::RSP_StrategyComment_USER_INPUT);
}

void CurrencyBseNse::o_place_order(int BuySell){
    BuyPriceNse = mktDataNse->getBidPrice(0);
    SellPriceNse = mktDataNse->getAskPrice(0);
    BuyPriceNse1 = mktDataNse->getBidPrice(1);
    SellPriceNse1 = mktDataNse->getAskPrice(1);

    if(FeedCount < 5){
        FeedCount++;
    }

    if(_IsBidding.compare("Yes") != 0 || BuyPriceNse <= 0 || SellPriceNse <= 0 || BuyPriceNse - BuyPriceNse1 >= 6*TickSize
            ||  SellPriceNse1 - SellPriceNse >= 6*TickSize || SellPriceNse - BuyPriceNse >= 16*TickSize)
    {
        //DEBUG_MESSAGE(reqQryDebugLog(), "Feed Error .... ");
        return;
    }

    if(_Depth == 0)
    {
        BuyQuotePrice = BuyPriceNse - _BuyDiff;
        SellQuotePrice = SellPriceNse + _SellDiff;
        BuySizeSize = mktDataNse->getBidQty(0)/1000;
        SellSizeSize = mktDataNse->getAskQty(0)/1000;
        OrderBuySize = FindMinimumSize(BuySizeSize/2, _MaxOrderLot);
        OrderSellSize = FindMinimumSize(SellSizeSize/2, _MaxOrderLot);
    }
    else if(_Depth == 1)
    {
        BuyQuotePrice = BuyPriceNse1 - _BuyDiff;
        SellQuotePrice = SellPriceNse1 + _SellDiff;
        BuySizeSize = mktDataNse->getBidQty(1)/1000;
        SellSizeSize = mktDataNse->getAskQty(1)/1000;
        OrderBuySize = FindMinimumSize(BuySizeSize/2, _MaxOrderLot);
        OrderSellSize = FindMinimumSize(SellSizeSize/2, _MaxOrderLot);
    }

    if(BuySell == 1){
        First_Buy_Pending_Qty = _orderWrapperBuy._instrument->getPosition()->getPendingQty(API2::CONSTANTS::CMD_OrderMode_BUY)/BoardLotSizeBse;
        Second_Sell_Pending_Qty = _FeedInstrumentNse->getPosition()->getPendingQty(API2::CONSTANTS::CMD_OrderMode_SELL)/BoardLotSizeNse;

        First_leg_BuyTraded = _BseTradeInstr -> getPosition()-> getTradedQty(API2::CONSTANTS::CMD_OrderMode_BUY)/BoardLotSizeBse;
        Second_leg_SellTraded = _FeedInstrumentNse -> getPosition()-> getTradedQty(API2::CONSTANTS::CMD_OrderMode_SELL)/BoardLotSizeNse;
        RemBuyQty = _BuyTotalLot - First_leg_BuyTraded;
        OrderBuySize = FindMinimumSize(OrderBuySize, RemBuyQty);

        if(BuyQuotePrice > 0 && FeedCount > 3 && First_leg_BuyTraded - Second_leg_SellTraded == 0 && First_Buy_Pending_Qty == 0 && Second_Sell_Pending_Qty == 0 && _BuyTotalLot >= First_leg_BuyTraded + OrderBuySize + _TotalBuyTraded)
        {
//            if(clock_gettime(CLOCK_REALTIME, &start) == -1){
//                perror("clock gettime");
//                exit( EXIT_FAILURE );
//            }
            _orderWrapperBuy.newOrder(risk, BuyQuotePrice,OrderBuySize*BoardLotSizeBse);
//            if(clock_gettime(CLOCK_REALTIME, &end) == -1){
//                perror("clock gettime");
//                exit( EXIT_FAILURE );
//            }
//            double diff = (end.tv_sec*1000000000 + end.tv_nsec) - (start.tv_sec*1000000000 + start.tv_nsec);
//            DEBUG_VARSHOW(reqQryDebugLog(), "o_place_order:: New Buy Order Send Time: ",diff/1000);
            OrderIdOrderWrapperSellMap.clear();
            OrderIdOrderWrapperSellMapSelfTrade.clear();
            FirstLegBuyTradePrice = 0;
            FirstLegBuyTradedQty = 0;
        }
    }
    else if(BuySell == 2){
        First_Sell_Pending_Qty = _orderWrapperSell._instrument->getPosition()->getPendingQty(API2::CONSTANTS::CMD_OrderMode_SELL)/BoardLotSizeBse;
        Second_Buy_Pending_Qty = _FeedInstrumentNse->getPosition()->getPendingQty(API2::CONSTANTS::CMD_OrderMode_BUY)/BoardLotSizeNse;

        First_leg_SellTraded = _BseTradeInstr -> getPosition()-> getTradedQty(API2::CONSTANTS::CMD_OrderMode_SELL)/BoardLotSizeBse;
        Second_leg_BuyTraded = _FeedInstrumentNse -> getPosition()-> getTradedQty(API2::CONSTANTS::CMD_OrderMode_BUY)/BoardLotSizeNse;
        RemSellQty = _SellTotalLot - First_leg_SellTraded;
        OrderSellSize = FindMinimumSize(OrderSellSize, RemSellQty);

        if(SellQuotePrice > 0 && FeedCount > 3 && Second_leg_BuyTraded - First_leg_SellTraded == 0 && First_Sell_Pending_Qty == 0 && Second_Buy_Pending_Qty == 0 && _SellTotalLot >= First_leg_SellTraded + OrderSellSize + _TotalSellTraded)
        {
//            if(clock_gettime(CLOCK_REALTIME, &start) == -1){
//                perror("clock gettime");
//                exit( EXIT_FAILURE );
//            }
            _orderWrapperSell.newOrder(risk,SellQuotePrice,OrderSellSize*BoardLotSizeBse);
//            if(clock_gettime(CLOCK_REALTIME, &end) == -1){
//                perror("clock gettime");
//                exit( EXIT_FAILURE );
//            }
//            double diff = (end.tv_sec*1000000000 + end.tv_nsec) - (start.tv_sec*1000000000 + start.tv_nsec);
//            DEBUG_VARSHOW(reqQryDebugLog(), "o_place_order:: New Sell Order Send Time: ",diff/1000);
            OrderIdOrderWrapperBuyMap.clear();
            OrderIdOrderWrapperBuyMapSelfTrade.clear();
            FirstLegSellTradePrice = 0;
            FirstLegSellTradedQty = 0;
        }
    }
}

void CurrencyBseNse::o_modify_cancel_order(int BuySell){
    BuyPriceNse = mktDataNse->getBidPrice(0);
    SellPriceNse = mktDataNse->getAskPrice(0);
    BuyPriceNse1 = mktDataNse->getBidPrice(1);
    SellPriceNse1 = mktDataNse->getAskPrice(1);
//    BuyPriceBse5 = mktDataBse->getBidPrice(4);
//    SellPriceBse5 = mktDataBse->getAskPrice(4);

    if(FeedCount < 5){
        FeedCount++;
    }

    if(_IsBidding.compare("Yes") != 0 || BuyPriceNse <= 0 || SellPriceNse <= 0 || BuyPriceNse - BuyPriceNse1 >= 6*TickSize
            ||  SellPriceNse1 - SellPriceNse >= 6*TickSize || SellPriceNse - BuyPriceNse >= 16*TickSize)
    {
        //DEBUG_MESSAGE(reqQryDebugLog(), "Feed Error .... ");
        if(BuySell == 1){
            _orderWrapperBuy.cancelOrder(risk);
        }
        else if(BuySell == 2){
            _orderWrapperSell.cancelOrder(risk);
        }
        return;
    }

    if(_Depth == 0)
    {
        BuyQuotePrice = BuyPriceNse - _BuyDiff;
        SellQuotePrice = SellPriceNse + _SellDiff;
        BuySizeSize = mktDataNse->getBidQty(0)/1000;
        SellSizeSize = mktDataNse->getAskQty(0)/1000;
        OrderBuySize = FindMinimumSize(BuySizeSize/2, _MaxOrderLot);
        OrderSellSize = FindMinimumSize(SellSizeSize/2, _MaxOrderLot);
    }
    else if(_Depth == 1)
    {
        BuyQuotePrice = BuyPriceNse1 - _BuyDiff;
        SellQuotePrice = SellPriceNse1 + _SellDiff;
        BuySizeSize = mktDataNse->getBidQty(1)/1000;
        SellSizeSize = mktDataNse->getAskQty(1)/1000;
        OrderBuySize = FindMinimumSize(BuySizeSize/2, _MaxOrderLot);
        OrderSellSize = FindMinimumSize(SellSizeSize/2, _MaxOrderLot);
    }

    if(BuySell == 1){
        First_Leg_BuyOrder_Price = _orderWrapperBuy._instrument->getLastQuotedPrice(API2::CONSTANTS::CMD_OrderMode_BUY);
//        if(BuyQuotePrice < BuyPriceBse5 && BuyQuotePrice > First_Leg_BuyOrder_Price){
//            //DEBUG_MESSAGE(reqQryDebugLog(), "o_modify_cancel_order:: Buy Quote is below 5th level Cancelling.");
//            return;
//        }

        First_Buy_Pending_Qty = _orderWrapperBuy._instrument->getPosition()->getPendingQty(API2::CONSTANTS::CMD_OrderMode_BUY)/BoardLotSizeBse;
        RemBuyQty = _BuyTotalLot - First_leg_BuyTraded;
        OrderBuySize = FindMinimumSize(OrderBuySize, RemBuyQty);

        if(First_Buy_Pending_Qty > 0 && First_Leg_BuyOrder_Price > 0)
        {
            cond = 0;
            if(cond == 0 && First_Leg_BuyOrder_Price != BuyQuotePrice && BuyQuotePrice > 0)
            {
//                if(clock_gettime(CLOCK_REALTIME, &start) == -1){
//                    perror("clock gettime");
//                    exit( EXIT_FAILURE );
//                }
                _orderWrapperBuy.replaceOrder(risk, BuyQuotePrice, (OrderBuySize*BoardLotSizeBse) + FirstLegBuyTradedQty);
//                if(clock_gettime(CLOCK_REALTIME, &end) == -1){
//                    perror("clock gettime");
//                    exit( EXIT_FAILURE );
//                }
//                double diff = (end.tv_sec*1000000000 + end.tv_nsec) - (start.tv_sec*1000000000 + start.tv_nsec);
//                DEBUG_VARSHOW(reqQryDebugLog(), "o_modify_cancel_order::1 Modify Buy Order Send Time: ",diff/1000);
                cond = 1;
            }
//            if(cond == 0 && ((OrderBuySize <= (std::floor(_MaxOrderLot/2))) || (OrderBuySize >= _MaxOrderLot)) && First_Buy_Pending_Qty != OrderBuySize ){
//                if(clock_gettime(CLOCK_REALTIME, &start) == -1){
//                    perror("clock gettime");
//                    exit( EXIT_FAILURE );
//                }
//                _orderWrapperBuy.replaceOrder(risk, 0, (OrderBuySize*BoardLotSizeBse) + FirstLegBuyTradedQty);
//                if(clock_gettime(CLOCK_REALTIME, &end) == -1){
//                    perror("clock gettime");
//                    exit( EXIT_FAILURE );
//                }
//                double diff = (end.tv_sec*1000000000 + end.tv_nsec) - (start.tv_sec*1000000000 + start.tv_nsec);
//                DEBUG_VARSHOW(reqQryDebugLog(), "o_modify_cancel_order::2 Modify Buy Order Send Time: ",diff/1000);
//                cond = 1;
//            }
//            if(cond == 0 && OrderBuySize < First_Buy_Pending_Qty ){
//                if(clock_gettime(CLOCK_REALTIME, &start) == -1){
//                    perror("clock gettime");
//                    exit( EXIT_FAILURE );
//                }
//                _orderWrapperBuy.replaceOrder(risk, 0, (OrderBuySize*BoardLotSizeBse) + FirstLegBuyTradedQty);
//                if(clock_gettime(CLOCK_REALTIME, &end) == -1){
//                    perror("clock gettime");
//                    exit( EXIT_FAILURE );
//                }
//                double diff = (end.tv_sec*1000000000 + end.tv_nsec) - (start.tv_sec*1000000000 + start.tv_nsec);
//                DEBUG_VARSHOW(reqQryDebugLog(), "o_modify_cancel_order::3 Modify Buy Order Send Time: ",diff/1000);
//                cond = 1;
//            }
        }
    }
    else if(BuySell == 2){
        First_Leg_SellOrder_Price = _orderWrapperSell._instrument->getLastQuotedPrice(API2::CONSTANTS::CMD_OrderMode_SELL);
//        if(SellQuotePrice > SellPriceBse5 && SellQuotePrice < First_Leg_SellOrder_Price){
//            //DEBUG_MESSAGE(reqQryDebugLog(), "o_modify_cancel_order:: Sell Quote is below 5th level Cancelling.");
//            return;
//        }

        First_Sell_Pending_Qty = _orderWrapperSell._instrument->getPosition()->getPendingQty(API2::CONSTANTS::CMD_OrderMode_SELL)/BoardLotSizeBse;
        RemSellQty = _SellTotalLot - First_leg_SellTraded;
        OrderSellSize = FindMinimumSize(OrderSellSize, RemSellQty);

        if(First_Sell_Pending_Qty > 0 && First_Leg_SellOrder_Price > 0)
        {
            cond = 0;
            if(cond == 0 && First_Leg_SellOrder_Price != SellQuotePrice && SellQuotePrice > 0)
            {
//                if(clock_gettime(CLOCK_REALTIME, &start) == -1){
//                    perror("clock gettime");
//                    exit( EXIT_FAILURE );
//                }
                _orderWrapperSell.replaceOrder(risk,SellQuotePrice,(OrderSellSize*BoardLotSizeBse)+ FirstLegSellTradedQty);
//                if(clock_gettime(CLOCK_REALTIME, &end) == -1){
//                    perror("clock gettime");
//                    exit( EXIT_FAILURE );
//                }
//                double diff = (end.tv_sec*1000000000 + end.tv_nsec) - (start.tv_sec*1000000000 + start.tv_nsec);
//                DEBUG_VARSHOW(reqQryDebugLog(), "o_modify_cancel_order::1 Modify Sell Order Send Time: ",diff/1000);
                cond = 1;
            }
//            if(cond == 0 && ((OrderSellSize <= (std::floor(_MaxOrderLot/2))) || (OrderSellSize >= _MaxOrderLot)) && First_Sell_Pending_Qty != OrderSellSize ){
//                if(clock_gettime(CLOCK_REALTIME, &start) == -1){
//                    perror("clock gettime");
//                    exit( EXIT_FAILURE );
//                }
//                _orderWrapperSell.replaceOrder(risk,0,(OrderSellSize*BoardLotSizeBse)+ FirstLegSellTradedQty);
//                if(clock_gettime(CLOCK_REALTIME, &end) == -1){
//                    perror("clock gettime");
//                    exit( EXIT_FAILURE );
//                }
//                double diff = (end.tv_sec*1000000000 + end.tv_nsec) - (start.tv_sec*1000000000 + start.tv_nsec);
//                DEBUG_VARSHOW(reqQryDebugLog(), "o_modify_cancel_order::2 Modify Sell Order Send Time: ",diff/1000);
//                cond = 1;
//            }
//            if(cond == 0 && OrderSellSize < First_Sell_Pending_Qty ){
//                if(clock_gettime(CLOCK_REALTIME, &start) == -1){
//                    perror("clock gettime");
//                    exit( EXIT_FAILURE );
//                }
//                _orderWrapperSell.replaceOrder(risk,0,(OrderSellSize*BoardLotSizeBse)+ FirstLegSellTradedQty);
//                if(clock_gettime(CLOCK_REALTIME, &end) == -1){
//                    perror("clock gettime");
//                    exit( EXIT_FAILURE );
//                }
//                double diff = (end.tv_sec*1000000000 + end.tv_nsec) - (start.tv_sec*1000000000 + start.tv_nsec);
//                DEBUG_VARSHOW(reqQryDebugLog(), "o_modify_cancel_order::3 Modify Sell Order Send Time: ",diff/1000);
//                cond = 1;
//            }
        }
    }
}

void CurrencyBseNse::o_modify_cancel_sec_order(int BuySell){
    SellPriceNse = mktDataNse->getAskPrice(0);
    BuyPriceNse = mktDataNse->getBidPrice(0);

    if(BuySell == 1){
        for(auto iter = OrderIdOrderWrapperBuyMap.begin(); iter != OrderIdOrderWrapperBuyMap.end(); ++iter)
        {
            std::pair<API2_NEW::COMMON::OrderWrapper*, API2::DATA_TYPES::PRICE> p = iter->second;
            API2_NEW::COMMON::OrderWrapper* _orderWrapperSecondBuy = p.first;
            API2::DATA_TYPES::QTY temp = _orderWrapperSecondBuy->_instrument->getPosition()->getPendingQty(API2::CONSTANTS::CMD_OrderMode_BUY)/BoardLotSizeBse;
            if(temp > 0){
                if(!(_orderWrapperSecondBuy->_isPendingNew || _orderWrapperSecondBuy->_isPendingReplace || _orderWrapperSecondBuy->_isPendingCancel)){
                    OrderPriceBuySecond = _orderWrapperSecondBuy->_instrument->getLastQuotedPrice(API2::CONSTANTS::CMD_OrderMode_BUY);
                    dTrigger = OrderPriceBuySecond + _TriggerPrice;
                    if(BuyPriceNse >= dTrigger && OrderPriceBuySecond > 0){
                        _orderWrapperSecondBuy->replaceOrder(risk,dTrigger + _StopLoss, 0);
                    }
                }
                else{
                    //DEBUG_MESSAGE(reqQryDebugLog(), "Second leg Buy is in Pending State");
                }
            }
        }

        for(auto iter = OrderIdOrderWrapperBuyMapSelfTrade.begin(); iter != OrderIdOrderWrapperBuyMapSelfTrade.end(); ++iter)
        {
            std::pair<API2_NEW::COMMON::OrderWrapper*, API2::DATA_TYPES::PRICE> p = iter->second;
            API2_NEW::COMMON::OrderWrapper* _orderWrapperSecondBuy = p.first;
            API2::DATA_TYPES::QTY temp = _orderWrapperSecondBuy->_instrument->getPosition()->getPendingQty(API2::CONSTANTS::CMD_OrderMode_BUY)/BoardLotSizeBse;
            SecLegBuyOrderP = p.second;
            if(temp > 0){
                if(!(_orderWrapperSecondBuy->_isPendingNew || _orderWrapperSecondBuy->_isPendingReplace || _orderWrapperSecondBuy->_isPendingCancel)){
                    OrderPriceBuySecond = _orderWrapperSecondBuy->_instrument->getLastQuotedPrice(API2::CONSTANTS::CMD_OrderMode_BUY);
                    dTrigger = OrderPriceBuySecond + _TriggerPrice;
                    iCond = 0;
                    if(BuyPriceNse >= dTrigger && OrderPriceBuySecond > 0)
                    {
                        _orderWrapperSecondBuy->replaceOrder(risk,dTrigger + _StopLoss, 0);
                        iCond = 1;
                    }

                    if(iCond == 0 && SellPriceNse <= SecLegBuyOrderP && OrderPriceBuySecond != SecLegBuyOrderP)
                    {
                        _orderWrapperSecondBuy->replaceOrder(risk,SecLegBuyOrderP, 0);
                        iCond = 1;
                    }

                    if(iCond == 0 && SellPriceNse > SecLegBuyOrderP)
                    {
                        if(SellPriceNse - OrderPriceBuySecond > TickSize)
                        {
                            NewPrice = OrderPriceBuySecond + TickSize;

                            if(NewPrice <= SecLegBuyOrderP)
                            {
                                _orderWrapperSecondBuy->replaceOrder(risk,NewPrice, 0);
                                iCond = 1;
                            }
                        }
                    }
                }
            }
        }
    }
    else if(BuySell == 2){
        for(auto iter = OrderIdOrderWrapperSellMap.begin(); iter != OrderIdOrderWrapperSellMap.end(); ++iter)
        {
            std::pair<API2_NEW::COMMON::OrderWrapper*, API2::DATA_TYPES::PRICE> p = iter->second;
            API2_NEW::COMMON::OrderWrapper* _orderWrapperSecondSell = p.first;
            API2::DATA_TYPES::QTY temp = _orderWrapperSecondSell->_instrument->getPosition()->getPendingQty(API2::CONSTANTS::CMD_OrderMode_SELL)/BoardLotSizeBse;
            if(temp > 0){
                if(!(_orderWrapperSecondSell->_isPendingNew || _orderWrapperSecondSell->_isPendingReplace || _orderWrapperSecondSell->_isPendingCancel)){
                    OrderPriceSellSecond = _orderWrapperSecondSell->_instrument->getLastQuotedPrice(API2::CONSTANTS::CMD_OrderMode_SELL);
                    dTrigger = OrderPriceSellSecond - _TriggerPrice;
                    if(SellPriceNse <= dTrigger && OrderPriceSellSecond > 0){
                       _orderWrapperSecondSell->replaceOrder(risk,dTrigger - _StopLoss,0);
                    }
                }
                else{
                    //DEBUG_MESSAGE(reqQryDebugLog(), "Second leg Sell is in Pending State");
                }
            }
        }

        for(auto iter = OrderIdOrderWrapperSellMapSelfTrade.begin(); iter != OrderIdOrderWrapperSellMapSelfTrade.end(); ++iter)
        {
            std::pair<API2_NEW::COMMON::OrderWrapper*, API2::DATA_TYPES::PRICE> p = iter->second;
            API2_NEW::COMMON::OrderWrapper* _orderWrapperSecondSell = p.first;
            API2::DATA_TYPES::QTY temp = _orderWrapperSecondSell->_instrument->getPosition()->getPendingQty(API2::CONSTANTS::CMD_OrderMode_SELL)/BoardLotSizeBse;
            SecLegSellOrderP = p.second;
            if(temp > 0){
                if(!(_orderWrapperSecondSell->_isPendingNew || _orderWrapperSecondSell->_isPendingReplace || _orderWrapperSecondSell->_isPendingCancel)){
                    OrderPriceSellSecond = _orderWrapperSecondSell->_instrument->getLastQuotedPrice(API2::CONSTANTS::CMD_OrderMode_SELL);
                    dTrigger = OrderPriceSellSecond - _TriggerPrice;
                    iCond = 0;
                    if(SellPriceNse <= dTrigger && OrderPriceSellSecond > 0)
                    {
                        _orderWrapperSecondSell->replaceOrder(risk,dTrigger - _StopLoss,0);
                        iCond = 1;
                    }
                    if(iCond == 0 && BuyPriceNse >= SecLegSellOrderP && OrderPriceSellSecond != SecLegSellOrderP)
                    {
                        _orderWrapperSecondSell->replaceOrder(risk,SecLegSellOrderP, 0);
                        iCond = 1;
                    }

                    if(iCond == 0 && BuyPriceNse < SecLegSellOrderP)
                    {
                        if(OrderPriceSellSecond - BuyPriceNse > TickSize)
                        {
                            NewPrice = OrderPriceSellSecond - TickSize;
                            if(NewPrice >= SecLegSellOrderP)
                            {
                                _orderWrapperSecondSell->replaceOrder(risk,NewPrice, 0);
                                iCond = 1;
                            }
                        }
                    }
                }
                else{
                    //DEBUG_MESSAGE(reqQryDebugLog(), "Second leg Self Trade Sell is in Pending State");
                }
            }
        }
    }
}


void CurrencyBseNse::onMarketDataEvent(UNSIGNED_LONG symbolId)
{
    //DEBUG_VARSHOW(reqQryDebugLog(), "Feed event---------------: ",symbolId);

    if(_orderWrapperBuy._isPendingNew || _orderWrapperBuy._isPendingReplace || _orderWrapperBuy._isPendingCancel){
        //DEBUG_MESSAGE(reqQryDebugLog(), "_orderWrapperBuy @ intermediate condition ");
    }
    else if(!_orderWrapperBuy._isReset){
        o_modify_cancel_order(1);
    }
    else if(_orderWrapperBuy._isReset){
        o_place_order(1);
    }

    if(_orderWrapperSell._isPendingNew || _orderWrapperSell._isPendingReplace || _orderWrapperSell._isPendingCancel)
    {
        //DEBUG_MESSAGE(reqQryDebugLog(), "_orderWrapperSell @ intermiadiate condition");
    }
    else if(!_orderWrapperSell._isReset){
        o_modify_cancel_order(2);
    }
    else if(_orderWrapperSell._isReset){
        o_place_order(2);
    }

    Second_Buy_Pending_Qty = _FeedInstrumentNse->getPosition()->getPendingQty(API2::CONSTANTS::CMD_OrderMode_BUY)/BoardLotSizeNse;
    if(Second_Buy_Pending_Qty > 0)
    {
        o_modify_cancel_sec_order(1);
    }

    Second_Sell_Pending_Qty = _FeedInstrumentNse->getPosition()->getPendingQty(API2::CONSTANTS::CMD_OrderMode_SELL)/BoardLotSizeNse;
    if(Second_Sell_Pending_Qty > 0)
    {
        o_modify_cancel_sec_order(2);
    }

//    DEBUG_FLUSH(reqQryDebugLog());
}

bool CurrencyBseNse::setParameters(API2::UserParams *params)
{
    FeedCount = 0;
    FlushCounter = 0;
    Error = 0;
    ErrorFlag = 0;
    FirstLegBuyTradePrice = 0;
    FirstLegSellTradePrice = 0;
    FirstLegBuyTradedQty = 0;
    FirstLegSellTradedQty = 0;
    ParamError = 0;
    FlushCounter = 0;
    _BuySide = API2::CONSTANTS::CMD_OrderMode_BUY;
    _SellSide = API2::CONSTANTS::CMD_OrderMode_SELL;

    SecLegBuyOrderP = 0;
    SecLegSellOrderP = 0;

    if(params->getValue("Acc Detail 1",accountDetailLeg1)!=API2::UserParamsError_OK)
    {
        DEBUG_MESSAGE(reqQryDebugLog(), "Issue in FirstLegAccount");
        DEBUG_FLUSH(reqQryDebugLog());
        ParamError = 1;
        return false ;
    }
    if(params->getValue("Acc Detail 2",accountDetailLeg2)!=API2::UserParamsError_OK)
    {
        DEBUG_MESSAGE(reqQryDebugLog(), "Issue in SecondLegAccount");
        DEBUG_FLUSH(reqQryDebugLog());
        ParamError = 1;
        return false;
    }

    if(params->getValue("Reset_All_Qty", Reset_All_Qty) != API2::UserParamsError_OK)
    {
        DEBUG_MESSAGE(reqQryDebugLog(), "Error in Reset_All_Qty Added in Portfolios ");
        DEBUG_FLUSH(reqQryDebugLog());
        ParamError = 1;
        return false;
    }


	if(params->getValue("SYMBOL LEG1", symbolId1) != API2::UserParamsError_OK || params->getValue("SYMBOL LEG2", symbolId2) != API2::UserParamsError_OK )
	{
        DEBUG_MESSAGE(reqQryDebugLog(), "Error in SymbolID Added in Portfolios ");
        DEBUG_FLUSH(reqQryDebugLog());
        ParamError = 1;
        return false;
	}

    if(params->getValue("TotalBuyTraded",TotalBuyTraded) != API2::UserParamsError_OK || params->getValue("TotalSellTraded",TotalSellTraded) != API2::UserParamsError_OK)
    {
        DEBUG_MESSAGE(reqQryDebugLog(), "issue in TotalBuyTraded OR TotalSellTraded");
        DEBUG_FLUSH(reqQryDebugLog());
        ParamError = 1;
        return false;
    }  

    if(Reset_All_Qty == 0)
    {
        _TotalBuyTraded = std::stoi(TotalBuyTraded);
        _TotalSellTraded = std::stoi(TotalSellTraded);
    }

    if(Reset_All_Qty == 1)
    {
        _TotalBuyTraded = std::stoi("0");
        _TotalSellTraded = std::stoi("0");

        Update_Buy_Position(0);
        Update_Sell_Position(0);
    }  

    if(params->getValue("BuyTotalLot",BuyTotalLot) != API2::UserParamsError_OK || params->getValue("SellTotalLot",SellTotalLot) != API2::UserParamsError_OK)
	{
        DEBUG_MESSAGE(reqQryDebugLog(), "issue in BuyTotal_Lot OR SellTotal_Lot");
        DEBUG_FLUSH(reqQryDebugLog());
        ParamError = 1;
        return false;
	}
	else
	{
        if(BuyTotalLot - _TotalBuyTraded > 1000 || SellTotalLot - _TotalSellTraded > 1000)
        {
            DEBUG_MESSAGE(reqQryDebugLog(), "Open BuyTotal_Lot OR SellTotal_Lot cant be more than 1000");
            DEBUG_FLUSH(reqQryDebugLog());
            ParamError = 1;
            return false;
        }
        else
        {
            _BuyTotalLot = BuyTotalLot;
            _SellTotalLot = SellTotalLot;
        }
	}   

    if(params->getValue("MaxOrderLot",MaxOrderLot) != API2::UserParamsError_OK)
	{
        DEBUG_MESSAGE(reqQryDebugLog(), "issue in Order_Lot");
        DEBUG_FLUSH(reqQryDebugLog());
        ParamError = 1;
        return false;
	}
	else
	{
        if(MaxOrderLot > 0 && MaxOrderLot < 50){
            _MaxOrderLot = MaxOrderLot;
        }
        else
        {
            DEBUG_MESSAGE(reqQryDebugLog(), "issue in MaxOrderLot entered into user Parameters.");
            DEBUG_FLUSH(reqQryDebugLog());
            ParamError = 1;
            return false;
        }
    }

    if(params->getValue("StopLoss",StopLoss1) != API2::UserParamsError_OK || params->getValue("TriggerPrice",TriggerPrice1) != API2::UserParamsError_OK || params->getValue("Tolerance",Tolerance1) != API2::UserParamsError_OK)
    {
        DEBUG_MESSAGE(reqQryDebugLog(), "issue in Stoploss / TriggerPrice / Tolerance  entered into user Parameters.");
        DEBUG_FLUSH(reqQryDebugLog());
        ParamError = 1;
        return false;
    }
    else
    {
        _StopLoss = StopLoss1;
        _TriggerPrice = TriggerPrice1;
        _Tolerance = Tolerance1;
    }  

    if(params->getValue("BuyDiff",BuyDiff1) != API2::UserParamsError_OK || params->getValue("SellDiff",SellDiff1) != API2::UserParamsError_OK)
    {
        DEBUG_MESSAGE(reqQryDebugLog(), "issue in Difference entered into user Parameters.");
        DEBUG_FLUSH(reqQryDebugLog());
        ParamError = 1;
        return false;
    }
    else
    {
        if(BuyDiff1 + SellDiff1 < 0)
        {
            DEBUG_MESSAGE(reqQryDebugLog(), "Sum of Buy/Sell Difference can't be less than Zero");
            DEBUG_FLUSH(reqQryDebugLog());
            ParamError = 1;
            return false;
        }
        else
        {
            if(BuyDiff1 < -100 || SellDiff1 < -100)
            {
                DEBUG_MESSAGE(reqQryDebugLog(), "Sum of BuyDiff1 < -0.0100 || SellDiff1 < 0.0100 ");
                DEBUG_FLUSH(reqQryDebugLog());
                ParamError = 1;
                return false;
            }
            else
            {
                _BuyDiff = BuyDiff1;
                _SellDiff = SellDiff1;
            }
        }
    }   

    if(params->getValue("IsBidding",IsBidding) != API2::UserParamsError_OK)
    {
        DEBUG_MESSAGE(reqQryDebugLog(), "issue in IsBidding");
        DEBUG_FLUSH(reqQryDebugLog());
        ParamError = 1;
        return false;
    }
    else
    {
        _IsBidding = IsBidding;
       //_IsBidding.copy(IsBidding);
    }

    if(params->getValue("Depth",Depth) != API2::UserParamsError_OK)
    {
        DEBUG_MESSAGE(reqQryDebugLog(), "issue in Depth ");
        DEBUG_FLUSH(reqQryDebugLog());
        ParamError = 1;
        return false;
    }
    else{
        _Depth = Depth;
    }

     _tradeInstrument1 = createNewInstrument(symbolId1,false, false);
     _tradeInstrument2 = createNewInstrument(symbolId2,false, false);

     Symbol1 = _tradeInstrument1->getStaticData()->symbol;
     Symbol2 = _tradeInstrument2->getStaticData()->symbol;
     Maturity1 = _tradeInstrument1->getStaticData()->maturityYearmon;
     Maturity2 = _tradeInstrument2->getStaticData()->maturityYearmon;
     MaturityDay1 = _tradeInstrument1->getStaticData()->maturityDay;
     MaturityDay2 = _tradeInstrument2->getStaticData()->maturityDay;
     DateDiff = MaturityDay1  - MaturityDay2;

     if(DateDiff < 0){
         DateDiff = -DateDiff;
     }

     DEBUG_VARSHOW(reqQryDebugLog(), "setParameters:: Symbol: ",Symbol1);
     if (Symbol1.compare(Symbol2) == 0)
     {
         if(Maturity1 != Maturity2 || DateDiff > 2)
         {
             DEBUG_MESSAGE(reqQryDebugLog(), "Expiry MisMatch... ");
             DEBUG_FLUSH(reqQryDebugLog());
             ParamError = 1;
             return false;
         }
     }
     else
     {
         DEBUG_MESSAGE(reqQryDebugLog(), "Symbols Mismatch in Portfolios ");
         DEBUG_FLUSH(reqQryDebugLog());
         ParamError = 1;
         return false;
     }


     if( int(_tradeInstrument1->getStaticData()->securityType) == API2::CONSTANTS::CMD_SecurityType_OPTION &&
          int(_tradeInstrument2->getStaticData()->securityType) == API2::CONSTANTS::CMD_SecurityType_OPTION   )
     {

         if( int(_tradeInstrument1->getStaticData()->optionMode)!= int(_tradeInstrument2->getStaticData()->optionMode) )
         {
             DEBUG_MESSAGE(reqQryDebugLog(), "Token Must be same Option Type... ");
             DEBUG_FLUSH(reqQryDebugLog());
             ParamError = 1;
             return false;
         }

         if( _tradeInstrument1->getStaticData()->strikePrice != _tradeInstrument2->getStaticData()->strikePrice  )
         {
             DEBUG_MESSAGE(reqQryDebugLog(), "Stike Must Be same in Portfolio... ");
             DEBUG_FLUSH(reqQryDebugLog());
             ParamError = 1;
             return false;
         }

     }
     else
     {
         if( int(_tradeInstrument1->getStaticData()->securityType) == API2::CONSTANTS::CMD_SecurityType_FUTURE &&
              int(_tradeInstrument2->getStaticData()->securityType) == API2::CONSTANTS::CMD_SecurityType_FUTURE)
         {

         }
         else
         {
             DEBUG_MESSAGE(reqQryDebugLog(), "Wrong Combination of Tokens in Portfolio... ");
             DEBUG_FLUSH(reqQryDebugLog());
             ParamError = 1;
             return false;
         }

     }

    if(_tradeInstrument1->getStaticData()->marketId == "NSECDS" && _tradeInstrument2->getStaticData()->marketId == "BSECDS")
    {
        _NseTradeInstr = _tradeInstrument1;
        _BseTradeInstr = _tradeInstrument2;        
        _FeedInstrumentNse = createNewInstrument(symbolId1,true, true, false);
        //_FeedInstrumentBse = createNewInstrument(symbolId2,true, true, false);
        BoardLotSizeNse = _NseTradeInstr->getStaticData()->marketLot;
        BoardLotSizeBse = _BseTradeInstr->getStaticData()->marketLot;
        AccountDetailNse = accountDetailLeg1;
        AccountDetailBse = accountDetailLeg2;
    }
    else
    {
      if(_tradeInstrument1->getStaticData()->marketId == "BSECDS" && _tradeInstrument2->getStaticData()->marketId == "NSECDS")
      {
          _NseTradeInstr = _tradeInstrument2;
          _BseTradeInstr = _tradeInstrument1;
          //_FeedInstrumentBse = createNewInstrument(symbolId1,true, true, false);
          _FeedInstrumentNse = createNewInstrument(symbolId2,true, true, false);
          BoardLotSizeNse = _NseTradeInstr->getStaticData()->marketLot;
          BoardLotSizeBse = _BseTradeInstr->getStaticData()->marketLot;
          AccountDetailNse = accountDetailLeg2;
          AccountDetailBse = accountDetailLeg1;
      }
      else
      {
          DEBUG_MESSAGE(reqQryDebugLog(), "Token combination doesnt match CDS-BCD ");
          DEBUG_FLUSH(reqQryDebugLog());
          ParamError = 1;
          return false;
      }
    }   

    TickSize = _FeedInstrumentNse->getStaticData()->tickSize;
    //mktDataBse = reqQryMarketData(_FeedInstrumentBse->getSymbolId());
    mktDataNse = reqQryMarketData(_FeedInstrumentNse->getSymbolId());
    _orderWrapperBuy = API2_NEW::COMMON::OrderWrapper(_BseTradeInstr, _BuySide, this, AccountDetailBse);
    _orderWrapperSell = API2_NEW::COMMON::OrderWrapper(_BseTradeInstr, _SellSide, this, AccountDetailBse);

    DEBUG_FLUSH(reqQryDebugLog());
    return true;
}

void CurrencyBseNse::onCanceled(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId)
{
    if(_orderWrapperBuy._orderId == orderId)
    {
         _orderWrapperBuy._isPendingCancel = false;
         _orderWrapperBuy._isPendingNew = false;
         _orderWrapperBuy._isPendingReplace = false;
         _orderWrapperBuy.reset();
         o_place_order(1);
    }

    if(_orderWrapperSell._orderId == orderId)
    {
         _orderWrapperSell._isPendingCancel = false;
         _orderWrapperSell._isPendingNew = false;
         _orderWrapperSell._isPendingReplace = false;
         _orderWrapperSell.reset();
         o_place_order(2);
    }

    auto iter2 = OrderIdOrderWrapperBuyMap.find(orderId);
    if(iter2 != OrderIdOrderWrapperBuyMap.end())
    {
        std::pair<API2_NEW::COMMON::OrderWrapper*, API2::DATA_TYPES::PRICE > p = iter2->second;
        API2_NEW::COMMON::OrderWrapper* _orderWrapperSecondBuy = p.first;
        SecLegBuyOrderP = _orderWrapperSecondBuy->getOrderPrice();
        API2::SingleOrder* order = reqQryOrder(confirmation.getClOrderId());
        SIGNED_LONG CancelQty = 0;

        if( order ){
            CancelQty = order->getQuantity() - order->getFilledQuantity();
        }

        if(SecLegBuyOrderP > 0)
        {
            API2_NEW::COMMON::OrderWrapper *_orderWrapperSecondBuy = new API2_NEW::COMMON::OrderWrapper(_FeedInstrumentNse,_BuySide,this,AccountDetailNse);
            _orderWrapperSecondBuy->reset();
            _orderWrapperSecondBuy->newOrder(risk,SecLegBuyOrderP - TickSize ,CancelQty);
            std::pair<API2_NEW::COMMON::OrderWrapper*, API2::DATA_TYPES::PRICE> p = std::make_pair(_orderWrapperSecondBuy,SecLegBuyOrderP);
            OrderIdOrderWrapperBuyMapSelfTrade[_orderWrapperSecondBuy->_orderId]= p;
            OrderIdOrderWrapperBuyMapSelfTrade.insert(std::pair<API2::COMMON::OrderId*,std::pair<API2_NEW::COMMON::OrderWrapper*, API2::DATA_TYPES::PRICE> >(_orderWrapperSecondBuy->_orderId,p));
        }
        OrderIdOrderWrapperBuyMap.erase(iter2);
    }

    auto iter3 = OrderIdOrderWrapperSellMap.find(orderId);
    if(iter3 != OrderIdOrderWrapperSellMap.end())
    {
        std::pair<API2_NEW::COMMON::OrderWrapper*, API2::DATA_TYPES::PRICE > p = iter3->second;
        API2_NEW::COMMON::OrderWrapper* _orderWrapperSecondSell = p.first;
        SecLegSellOrderP = _orderWrapperSecondSell->getOrderPrice();
        API2::SingleOrder* order = reqQryOrder(confirmation.getClOrderId());
        SIGNED_LONG CancelQty = 0;

        if( order ){
            CancelQty = order->getQuantity() - order->getFilledQuantity();
        }

        if(SecLegSellOrderP > 0)
        {
            API2_NEW::COMMON::OrderWrapper *_orderWrapperSecondSell = new API2_NEW::COMMON::OrderWrapper(_FeedInstrumentNse,_SellSide,this,AccountDetailNse);
            _orderWrapperSecondSell->reset();
            _orderWrapperSecondSell->newOrder(risk,SecLegSellOrderP + TickSize ,CancelQty);
            std::pair<API2_NEW::COMMON::OrderWrapper*, API2::DATA_TYPES::PRICE> p = std::make_pair(_orderWrapperSecondSell,SecLegSellOrderP);
            OrderIdOrderWrapperSellMapSelfTrade[_orderWrapperSecondSell->_orderId]= p;
            OrderIdOrderWrapperSellMapSelfTrade.insert(std::pair<API2::COMMON::OrderId*,std::pair<API2_NEW::COMMON::OrderWrapper*, API2::DATA_TYPES::PRICE> >(_orderWrapperSecondSell->_orderId,p));
        }
        OrderIdOrderWrapperSellMap.erase(iter3);
    }

    auto iter4 = OrderIdOrderWrapperBuyMapSelfTrade.find(orderId);
    if(iter4 != OrderIdOrderWrapperBuyMapSelfTrade.end())
    {
        std::pair<API2_NEW::COMMON::OrderWrapper*, API2::DATA_TYPES::PRICE > p = iter4->second;
        SecLegBuyOrderP = p.second;
        API2::SingleOrder* order = reqQryOrder(confirmation.getClOrderId());
        SIGNED_LONG CancelQty = 0;

        if( order ){
            CancelQty = order->getQuantity() - order->getFilledQuantity();
        }

        if(SecLegBuyOrderP > 0)
        {
            API2_NEW::COMMON::OrderWrapper *_orderWrapperSecondBuy = new API2_NEW::COMMON::OrderWrapper(_FeedInstrumentNse,_BuySide,this,AccountDetailNse);
            _orderWrapperSecondBuy->reset();
            _orderWrapperSecondBuy->newOrder(risk,p.first->getOrderPrice() - TickSize ,CancelQty);
            std::pair<API2_NEW::COMMON::OrderWrapper*, API2::DATA_TYPES::PRICE> p = std::make_pair(_orderWrapperSecondBuy,SecLegBuyOrderP);
            OrderIdOrderWrapperBuyMapSelfTrade[_orderWrapperSecondBuy->_orderId]= p;
            OrderIdOrderWrapperBuyMapSelfTrade.insert(std::pair<API2::COMMON::OrderId*,std::pair<API2_NEW::COMMON::OrderWrapper*, API2::DATA_TYPES::PRICE> >(_orderWrapperSecondBuy->_orderId,p));
        }
        OrderIdOrderWrapperBuyMapSelfTrade.erase(iter4);
    }

    auto iter5 = OrderIdOrderWrapperSellMapSelfTrade.find(orderId);
    if(iter5 != OrderIdOrderWrapperSellMapSelfTrade.end())
    {
        std::pair<API2_NEW::COMMON::OrderWrapper*, API2::DATA_TYPES::PRICE > p = iter5->second;
        SecLegSellOrderP = p.second;
        API2::SingleOrder* order = reqQryOrder(confirmation.getClOrderId());
        SIGNED_LONG CancelQty = 0;

        if( order ){
            CancelQty = order->getQuantity() - order->getFilledQuantity();
        }

        if(SecLegSellOrderP > 0)
        {
            API2_NEW::COMMON::OrderWrapper *_orderWrapperSecondSell = new API2_NEW::COMMON::OrderWrapper(_FeedInstrumentNse,_SellSide,this,AccountDetailNse);
            _orderWrapperSecondSell->reset();
            _orderWrapperSecondSell->newOrder(risk,p.first->getOrderPrice() + TickSize ,CancelQty);
            std::pair<API2_NEW::COMMON::OrderWrapper*, API2::DATA_TYPES::PRICE> p = std::make_pair(_orderWrapperSecondSell,SecLegSellOrderP);
            OrderIdOrderWrapperSellMapSelfTrade[_orderWrapperSecondSell->_orderId]= p;
            OrderIdOrderWrapperSellMapSelfTrade.insert(std::pair<API2::COMMON::OrderId*,std::pair<API2_NEW::COMMON::OrderWrapper*, API2::DATA_TYPES::PRICE> >(_orderWrapperSecondSell->_orderId,p));
        }
        OrderIdOrderWrapperSellMapSelfTrade.erase(iter5);
    }
}

void CurrencyBseNse::onReplaced(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId)
{
    if(_orderWrapperBuy._orderId == orderId)
    {
        if(!_orderWrapperBuy.processConfirmation(confirmation))
        {
            _orderWrapperBuy._isPendingCancel = false;
            _orderWrapperBuy._isPendingNew = false;
            _orderWrapperBuy._isPendingReplace = false;
        }
        else{
            o_modify_cancel_order(1);
        }
    }

    if(_orderWrapperSell._orderId == orderId)
    {
        if(!_orderWrapperSell.processConfirmation(confirmation))
        {
            _orderWrapperSell._isPendingCancel = false;
            _orderWrapperSell._isPendingNew = false;
            _orderWrapperSell._isPendingReplace = false;
        }
        else{
            o_modify_cancel_order(2);
        }
    }

    if(OrderIdOrderWrapperBuyMap.find(orderId) != OrderIdOrderWrapperBuyMap.end()){
        API2_NEW::COMMON::OrderWrapper* _orderWrapperSecondBuy = OrderIdOrderWrapperBuyMap[orderId].first;
        if(!_orderWrapperSecondBuy->processConfirmation(confirmation))
        {
            //DEBUG_MESSAGE(reqQryDebugLog(),"Order Wrapper Process Confirmation Failed _orderWrapperSecondBuy");
        }
        else{
            o_modify_cancel_sec_order(1);
        }
    }

    if(OrderIdOrderWrapperSellMap.find(orderId) != OrderIdOrderWrapperSellMap.end()){
        API2_NEW::COMMON::OrderWrapper* _orderWrapperSecondSell = OrderIdOrderWrapperSellMap[orderId].first;
        if(!_orderWrapperSecondSell->processConfirmation(confirmation))
        {
            //DEBUG_MESSAGE(reqQryDebugLog(),"Order Wrapper Process Confirmation Failed _orderWrapperSecondSell");
        }
        else{
            o_modify_cancel_sec_order(2);
        }
    }

    if(OrderIdOrderWrapperBuyMapSelfTrade.find(orderId) != OrderIdOrderWrapperBuyMapSelfTrade.end()){
        API2_NEW::COMMON::OrderWrapper* _orderWrapperSecondBuy = OrderIdOrderWrapperBuyMapSelfTrade[orderId].first;
        if(!_orderWrapperSecondBuy->processConfirmation(confirmation))
        {
            //DEBUG_MESSAGE(reqQryDebugLog(),"Order Wrapper Process Confirmation Failed _orderWrapperSecondBuySelfTrade");
        }
        else{
            o_modify_cancel_sec_order(1);
        }
    }

    if(OrderIdOrderWrapperSellMapSelfTrade.find(orderId) != OrderIdOrderWrapperSellMapSelfTrade.end()){
        API2_NEW::COMMON::OrderWrapper* _orderWrapperSecondSell = OrderIdOrderWrapperSellMapSelfTrade[orderId].first;
        if(!_orderWrapperSecondSell->processConfirmation(confirmation))
        {
            //DEBUG_MESSAGE(reqQryDebugLog(),"Order Wrapper Process Confirmation Failed _orderWrapperSecondSellSelfTrade");
        }
        else{
            o_modify_cancel_sec_order(2);
        }
    }
}

void CurrencyBseNse::onConfirmed(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId)
{
    if(_orderWrapperBuy._orderId == orderId)
    {       
        if(!_orderWrapperBuy.processConfirmation(confirmation))
        {
            DEBUG_MESSAGE(reqQryDebugLog(),"Order Wrapper Process Confirmation Failed _orderWrapperBuy");
        }
        else{
            o_modify_cancel_order(1);
        }

    }

    if(_orderWrapperSell._orderId == orderId)
    {
        if(!_orderWrapperSell.processConfirmation(confirmation))
        {
            DEBUG_MESSAGE(reqQryDebugLog(),"Order Wrapper Process Confirmation Failed _orderWrapperSell");
        }
        else{
            o_modify_cancel_order(2);
        }
    }

    if(OrderIdOrderWrapperBuyMap.find(orderId) != OrderIdOrderWrapperBuyMap.end()){
        API2_NEW::COMMON::OrderWrapper* _orderWrapperSecondBuy = OrderIdOrderWrapperBuyMap[orderId].first;
        if(!_orderWrapperSecondBuy->processConfirmation(confirmation))
        {
            //DEBUG_MESSAGE(reqQryDebugLog(),"Order Wrapper Process Confirmation Failed _orderWrapperSecondBuy");
        }
        else{
            o_modify_cancel_sec_order(1);
        }
    }

    if(OrderIdOrderWrapperSellMap.find(orderId) != OrderIdOrderWrapperSellMap.end()){
        API2_NEW::COMMON::OrderWrapper* _orderWrapperSecondSell = OrderIdOrderWrapperSellMap[orderId].first;
        if(!_orderWrapperSecondSell->processConfirmation(confirmation))
        {
            //DEBUG_MESSAGE(reqQryDebugLog(),"Order Wrapper Process Confirmation Failed _orderWrapperSecondSell");
        }
        else{
            o_modify_cancel_sec_order(2);
        }
    }

    if(OrderIdOrderWrapperBuyMapSelfTrade.find(orderId) != OrderIdOrderWrapperBuyMapSelfTrade.end()){
        API2_NEW::COMMON::OrderWrapper* _orderWrapperSecondBuy = OrderIdOrderWrapperBuyMapSelfTrade[orderId].first;
        if(!_orderWrapperSecondBuy->processConfirmation(confirmation))
        {
            //DEBUG_MESSAGE(reqQryDebugLog(),"Order Wrapper Process Confirmation Failed _orderWrapperSecondBuySelfTrade");
        }
        else{
            o_modify_cancel_sec_order(1);
        }
    }

    if(OrderIdOrderWrapperSellMapSelfTrade.find(orderId) != OrderIdOrderWrapperSellMapSelfTrade.end()){
        API2_NEW::COMMON::OrderWrapper* _orderWrapperSecondSell = OrderIdOrderWrapperSellMapSelfTrade[orderId].first;
        if(!_orderWrapperSecondSell->processConfirmation(confirmation))
        {
            //DEBUG_MESSAGE(reqQryDebugLog(),"Order Wrapper Process Confirmation Failed _orderWrapperSecondSellSelfTrade");
        }
        else{
            o_modify_cancel_sec_order(2);
        }
    }
}

void CurrencyBseNse::onFilled(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId)
{
    API2::DATA_TYPES::RiskStatus risk;

    if(API2::DATA_TYPES::SYMBOL_ID(_BseTradeInstr->getSymbolId()) == confirmation.getSymbolId())
    {
        TrdQty = (confirmation.getLastFillQuantity()/BoardLotSizeBse)*BoardLotSizeNse;
        TrdPrice = confirmation.getLastFillPrice();

        if(confirmation.getOrderMode() == API2::CONSTANTS::CMD_OrderMode_BUY)
        {
            API2_NEW::COMMON::OrderWrapper *_orderWrapperSecondSell = new API2_NEW::COMMON::OrderWrapper(_FeedInstrumentNse,_SellSide,this,AccountDetailNse);
            _orderWrapperSecondSell->reset();
            std::pair<API2_NEW::COMMON::OrderWrapper*, API2::DATA_TYPES::PRICE> p = std::make_pair(_orderWrapperSecondSell,0);
            OrderIdOrderWrapperSellMap[_orderWrapperSecondSell->_orderId]= p;
            _orderWrapperSecondSell->newOrder(risk,TrdPrice + _BuyDiff ,TrdQty);
            OrderIdOrderWrapperSellMap.insert(std::pair<API2::COMMON::OrderId*,std::pair<API2_NEW::COMMON::OrderWrapper*, API2::DATA_TYPES::PRICE> >(_orderWrapperSecondSell->_orderId,p));
            FirstLegBuyTradePrice = TrdPrice;
            FirstLegBuyTradedQty += confirmation.getLastFillQuantity();
            DEBUG_VARSHOW(reqQryDebugLog(), "onFilled:: First Leg Buy Traded at Price: ",TrdPrice);
            DEBUG_VARSHOW(reqQryDebugLog(), "onFilled:: First Leg Buy Traded at Qty: ",confirmation.getLastFillQuantity()/BoardLotSizeBse);
        }

        if(confirmation.getOrderMode() == API2::CONSTANTS::CMD_OrderMode_SELL)
        {
            API2_NEW::COMMON::OrderWrapper *_orderWrapperSecondBuy = new API2_NEW::COMMON::OrderWrapper(_FeedInstrumentNse,_BuySide,this,AccountDetailNse);
            _orderWrapperSecondBuy->reset();
            std::pair<API2_NEW::COMMON::OrderWrapper*, API2::DATA_TYPES::PRICE> p = std::make_pair(_orderWrapperSecondBuy,0);
            OrderIdOrderWrapperBuyMap[_orderWrapperSecondBuy->_orderId]= p;
            _orderWrapperSecondBuy->newOrder(risk,TrdPrice - _SellDiff ,TrdQty);
            OrderIdOrderWrapperBuyMap.insert(std::pair<API2::COMMON::OrderId*,std::pair<API2_NEW::COMMON::OrderWrapper*, API2::DATA_TYPES::PRICE> >(_orderWrapperSecondBuy->_orderId,p));
            FirstLegSellTradePrice = TrdPrice;
            FirstLegSellTradedQty += confirmation.getLastFillQuantity();
            DEBUG_VARSHOW(reqQryDebugLog(), "onFilled:: First Leg Sell Traded at Price: ",TrdPrice);
            DEBUG_VARSHOW(reqQryDebugLog(), "onFilled:: First Leg Sell Traded at Qty: ",confirmation.getLastFillQuantity()/BoardLotSizeBse);
        }
	}

    if(API2::DATA_TYPES::SYMBOL_ID(_FeedInstrumentNse->getSymbolId()) == confirmation.getSymbolId())
	{
        TrdPrice = confirmation.getLastFillPrice();
        TrdQty = confirmation.getLastFillQuantity();

        if(confirmation.getOrderMode() == API2::CONSTANTS::CMD_OrderMode_BUY)
        {
            First_Sell_Traded_Price = FirstLegSellTradePrice;
            Second_Buy_Traded_Price = TrdPrice;
            dTradeDiff = First_Sell_Traded_Price - Second_Buy_Traded_Price;

            if(dTradeDiff < (_SellDiff - _Tolerance))
            {
                 _IsBidding = "No";
                 Update_Text(1);
            }

            First_leg_Sell_Position = _BseTradeInstr -> getPosition()-> getTradedQty(API2::CONSTANTS::CMD_OrderMode_SELL)/BoardLotSizeBse;
            Second_leg_Buy_Position = _FeedInstrumentNse -> getPosition()-> getTradedQty(API2::CONSTANTS::CMD_OrderMode_BUY)/BoardLotSizeNse;
            First_Sell_Pending_Qty = _orderWrapperSell._instrument->getPosition()->getPendingQty(API2::CONSTANTS::CMD_OrderMode_SELL)/BoardLotSizeBse;

            Update_Sell_Position(First_leg_Sell_Position);

            if(First_leg_Sell_Position == Second_leg_Buy_Position)
            {
                OrderIdOrderWrapperBuyMap.clear();
                OrderIdOrderWrapperBuyMapSelfTrade.clear();
                o_modify_cancel_order(2);
            }

            if(First_Sell_Pending_Qty <= 0)
            {
                _orderWrapperSell._isPendingCancel = false;
                _orderWrapperSell._isPendingNew = false;
                _orderWrapperSell._isPendingReplace = false;
                _orderWrapperSell.reset();
                o_place_order(2);

            }

            FlushCounter++;
            if(FlushCounter == 5){
                DEBUG_FLUSH(reqQryDebugLog());
                FlushCounter = 0;
            }
        }

        if(confirmation.getOrderMode() == API2::CONSTANTS::CMD_OrderMode_SELL)
        {
            First_Buy_Traded_Price = FirstLegBuyTradePrice ;
            Second_Sell_Traded_Price = TrdPrice;
            dTradeDiff = Second_Sell_Traded_Price - First_Buy_Traded_Price;

            if(dTradeDiff < (_BuyDiff - _Tolerance))
            {
                 _IsBidding = "No";
                 Update_Text(1);
            }

            First_leg_Buy_Position = _BseTradeInstr -> getPosition()-> getTradedQty(API2::CONSTANTS::CMD_OrderMode_BUY)/BoardLotSizeBse;
            Second_leg_Sell_Position = _FeedInstrumentNse -> getPosition()-> getTradedQty(API2::CONSTANTS::CMD_OrderMode_SELL)/BoardLotSizeNse;

            First_Buy_Pending_Qty = _orderWrapperBuy._instrument->getPosition()->getPendingQty(API2::CONSTANTS::CMD_OrderMode_BUY)/BoardLotSizeBse;
            Update_Buy_Position(First_leg_Buy_Position);

            if(First_leg_Buy_Position == Second_leg_Sell_Position)
            {
                OrderIdOrderWrapperSellMap.clear();
                OrderIdOrderWrapperSellMapSelfTrade.clear();
                o_modify_cancel_order(1);
            }

            if(First_Buy_Pending_Qty <= 0){
                _orderWrapperBuy._isPendingCancel = false;
                _orderWrapperBuy._isPendingNew = false;
                _orderWrapperBuy._isPendingReplace = false;
                _orderWrapperBuy.reset();
                o_place_order(1);
            }

            FlushCounter++;
            if(FlushCounter == 5){
                DEBUG_FLUSH(reqQryDebugLog());
                FlushCounter = 0;
            }
        }
    }
}

void CurrencyBseNse::onPartialFill(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId)
{
    API2::DATA_TYPES::RiskStatus risk;

    if(API2::DATA_TYPES::SYMBOL_ID(_BseTradeInstr->getSymbolId()) == confirmation.getSymbolId())
    {
        TrdQty = (confirmation.getLastFillQuantity()/BoardLotSizeBse)*BoardLotSizeNse;
        TrdPrice = confirmation.getLastFillPrice();

        if(confirmation.getOrderMode() == API2::CONSTANTS::CMD_OrderMode_BUY)
        {
            API2_NEW::COMMON::OrderWrapper *_orderWrapperSecondSell = new API2_NEW::COMMON::OrderWrapper(_FeedInstrumentNse,_SellSide,this,AccountDetailNse);
            _orderWrapperSecondSell->reset();
            std::pair<API2_NEW::COMMON::OrderWrapper*, API2::DATA_TYPES::PRICE> p = std::make_pair(_orderWrapperSecondSell,0);
            OrderIdOrderWrapperSellMap[_orderWrapperSecondSell->_orderId]= p;
            _orderWrapperSecondSell->newOrder(risk,TrdPrice + _BuyDiff ,TrdQty);
            OrderIdOrderWrapperSellMap.insert(std::pair<API2::COMMON::OrderId*,std::pair<API2_NEW::COMMON::OrderWrapper*, API2::DATA_TYPES::PRICE> >(_orderWrapperSecondSell->_orderId,p));
            FirstLegBuyTradePrice = TrdPrice;
            FirstLegBuyTradedQty += confirmation.getLastFillQuantity();
            o_modify_cancel_order(1);
            DEBUG_VARSHOW(reqQryDebugLog(), "onPartialFill:: First Leg Buy Traded at Price: ",TrdPrice);
            DEBUG_VARSHOW(reqQryDebugLog(), "onPartialFill:: First Leg Buy Traded at Qty: ",confirmation.getLastFillQuantity()/BoardLotSizeBse);
        }

        if(confirmation.getOrderMode() == API2::CONSTANTS::CMD_OrderMode_SELL)
        {
            API2_NEW::COMMON::OrderWrapper *_orderWrapperSecondBuy = new API2_NEW::COMMON::OrderWrapper(_FeedInstrumentNse,_BuySide,this,AccountDetailNse);
            _orderWrapperSecondBuy->reset();
            std::pair<API2_NEW::COMMON::OrderWrapper*, API2::DATA_TYPES::PRICE> p = std::make_pair(_orderWrapperSecondBuy,0);
            OrderIdOrderWrapperBuyMap[_orderWrapperSecondBuy->_orderId]= p;
            _orderWrapperSecondBuy->newOrder(risk,TrdPrice - _SellDiff ,TrdQty);
            OrderIdOrderWrapperBuyMap.insert(std::pair<API2::COMMON::OrderId*,std::pair<API2_NEW::COMMON::OrderWrapper*, API2::DATA_TYPES::PRICE> >(_orderWrapperSecondBuy->_orderId,p));
            FirstLegSellTradePrice = TrdPrice;
            FirstLegSellTradedQty += confirmation.getLastFillQuantity();
            o_modify_cancel_order(2);
            DEBUG_VARSHOW(reqQryDebugLog(), "onPartialFill:: First Leg Sell Traded at Price: ",TrdPrice);
            DEBUG_VARSHOW(reqQryDebugLog(), "onPartialFill:: First Leg Sell Traded at Qty: ",confirmation.getLastFillQuantity()/BoardLotSizeBse);
        }
    }

    if(API2::DATA_TYPES::SYMBOL_ID(_FeedInstrumentNse->getSymbolId()) == confirmation.getSymbolId())
    {
        TrdPrice = confirmation.getLastFillPrice();
        TrdQty = confirmation.getLastFillQuantity();

        if(confirmation.getOrderMode() == API2::CONSTANTS::CMD_OrderMode_BUY)
        {
            First_Sell_Traded_Price = FirstLegSellTradePrice;
            Second_Buy_Traded_Price = TrdPrice;
            dTradeDiff = First_Sell_Traded_Price - Second_Buy_Traded_Price;

            if(dTradeDiff < (_SellDiff - _Tolerance))
            {
                _IsBidding = "No";
                Update_Text(1);
            }
        }

        if(confirmation.getOrderMode() == API2::CONSTANTS::CMD_OrderMode_SELL)
        {
            First_Buy_Traded_Price = FirstLegBuyTradePrice;
            Second_Sell_Traded_Price = TrdPrice;
            dTradeDiff = Second_Sell_Traded_Price - First_Buy_Traded_Price;

            if(dTradeDiff < (_BuyDiff - _Tolerance))
            {
                _IsBidding = "No";
                Update_Text(1);
            }
        }
    }
}

void CurrencyBseNse::onCancelRejected(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId)
{
    if( _orderWrapperBuy._orderId == orderId)
    {
        _orderWrapperBuy._isPendingCancel = false;
        _orderWrapperBuy._isPendingNew = false;
        _orderWrapperBuy._isPendingReplace = false;
        _orderWrapperBuy.reset();
    }

    if( _orderWrapperSell._orderId == orderId)
    { 
        _orderWrapperSell._isPendingCancel = false;
        _orderWrapperSell._isPendingNew = false;
        _orderWrapperSell._isPendingReplace = false;
        _orderWrapperSell.reset();
    }
}

void CurrencyBseNse::onReplaceRejected(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId)
{
    API2::DATA_TYPES::RiskStatus risk;
    if( _orderWrapperBuy._orderId == orderId)
    {
        _orderWrapperBuy._isPendingCancel = false;
        _orderWrapperBuy._isPendingNew = false;
        _orderWrapperBuy._isPendingReplace = false;
        _orderWrapperBuy.reset(); 
        First_Buy_Pending_Qty = _orderWrapperBuy._instrument->getPosition()->getPendingQty(API2::CONSTANTS::CMD_OrderMode_BUY)/BoardLotSizeBse;
        if(First_Buy_Pending_Qty > 0){
            o_modify_cancel_order(1);
        }
        else{
            _orderWrapperBuy.cancelOrder(risk);
        }
    }

    if( _orderWrapperSell._orderId == orderId)
    {     
        _orderWrapperSell._isPendingCancel = false;
        _orderWrapperSell._isPendingNew = false;
        _orderWrapperSell._isPendingReplace = false;
        _orderWrapperSell.reset();
        First_Sell_Pending_Qty = _orderWrapperSell._instrument->getPosition()->getPendingQty(API2::CONSTANTS::CMD_OrderMode_SELL)/BoardLotSizeBse;
        if(First_Sell_Pending_Qty > 0){
            o_modify_cancel_order(2);
        }
        else{
            _orderWrapperSell.cancelOrder(risk);
        }
    }

    if(OrderIdOrderWrapperBuyMap.find(orderId) != OrderIdOrderWrapperBuyMap.end()){
        API2_NEW::COMMON::OrderWrapper* _orderWrapperSecondBuy = OrderIdOrderWrapperBuyMap[orderId].first;
        if(!_orderWrapperSecondBuy->processConfirmation(confirmation))
        {
            //DEBUG_MESSAGE(reqQryDebugLog(),"Order Wrapper Process Confirmation Failed _orderWrapperSecondBuy");
        }
        else{
            Second_Buy_Pending_Qty = _orderWrapperSecondBuy->_instrument->getPosition()->getPendingQty(API2::CONSTANTS::CMD_OrderMode_BUY)/BoardLotSizeBse;
            if(Second_Buy_Pending_Qty > 0){
                o_modify_cancel_sec_order(1);
            }
        }
    }

    if(OrderIdOrderWrapperSellMap.find(orderId) != OrderIdOrderWrapperSellMap.end()){
        API2_NEW::COMMON::OrderWrapper* _orderWrapperSecondSell = OrderIdOrderWrapperSellMap[orderId].first;
        if(!_orderWrapperSecondSell->processConfirmation(confirmation))
        {
            //DEBUG_MESSAGE(reqQryDebugLog(),"Order Wrapper Process Confirmation Failed _orderWrapperSecondSell");
        }
        else{
            Second_Sell_Pending_Qty = _orderWrapperSecondSell->_instrument->getPosition()->getPendingQty(API2::CONSTANTS::CMD_OrderMode_SELL)/BoardLotSizeBse;
            if(Second_Buy_Pending_Qty > 0){
                o_modify_cancel_sec_order(2);
            }
        }
    }

    if(OrderIdOrderWrapperBuyMapSelfTrade.find(orderId) != OrderIdOrderWrapperBuyMapSelfTrade.end()){
        API2_NEW::COMMON::OrderWrapper* _orderWrapperSecondBuy = OrderIdOrderWrapperBuyMapSelfTrade[orderId].first;
        if(!_orderWrapperSecondBuy->processConfirmation(confirmation))
        {
            //DEBUG_MESSAGE(reqQryDebugLog(),"Order Wrapper Process Confirmation Failed _orderWrapperSecondBuySelfTrade");
        }
        else{
            Second_Buy_Pending_Qty = _orderWrapperSecondBuy->_instrument->getPosition()->getPendingQty(API2::CONSTANTS::CMD_OrderMode_BUY)/BoardLotSizeBse;
            if(Second_Buy_Pending_Qty > 0){
                o_modify_cancel_sec_order(1);
            }
        }
    }

    if(OrderIdOrderWrapperSellMapSelfTrade.find(orderId) != OrderIdOrderWrapperSellMapSelfTrade.end()){
        API2_NEW::COMMON::OrderWrapper* _orderWrapperSecondSell = OrderIdOrderWrapperSellMapSelfTrade[orderId].first;
        if(!_orderWrapperSecondSell->processConfirmation(confirmation))
        {
            //DEBUG_MESSAGE(reqQryDebugLog(),"Order Wrapper Process Confirmation Failed _orderWrapperSecondSellSelfTrade");
        }
        else{
            Second_Sell_Pending_Qty = _orderWrapperSecondSell->_instrument->getPosition()->getPendingQty(API2::CONSTANTS::CMD_OrderMode_SELL)/BoardLotSizeBse;
            if(Second_Buy_Pending_Qty > 0){
                o_modify_cancel_sec_order(2);
            }
        }
    }
}

void CurrencyBseNse::onNewReject(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId)
{
    DEBUG_FLUSH(reqQryDebugLog());
    reqTerminateStrategy();
}

void CurrencyBseNse::TestDriveStrategy(void *params)
{
	API2::StrategyParameters *sgParams = (API2::StrategyParameters*)params;
    boost::shared_ptr<API2::SGContext> context(new CurrencyBseNse(sgParams));
	context->reqStartAlgo(true,false);
	API2::SGContext::registerStrategy(context);   
}
