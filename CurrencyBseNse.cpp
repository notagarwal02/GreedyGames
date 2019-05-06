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
        DEBUG_MESSAGE(reqQryDebugLog(),"Exception in Instrument");           
        reqAddStrategyComment(API2::CONSTANTS::RSP_StrategyComment_STRATEGY_ERROR_STATE);
        DEBUG_FLUSH(reqQryDebugLog());
        reqTerminateStrategy();
        return;
    }
    catch(std::exception &e)
    {
        DEBUG_MESSAGE(reqQryDebugLog(),"Default Exception Raised");            
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
        DEBUG_MESSAGE(reqQryDebugLog(),"Either Instrument Is Null");               
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
        DEBUG_MESSAGE(reqQryDebugLog(),"Not a Valid Algo Id" );
        reqAddStrategyComment(API2::CONSTANTS::RSP_StrategyComment_STRATEGY_ERROR_STATE);
        DEBUG_FLUSH(reqQryDebugLog());
        reqTerminateStrategy();
        return ;
    }

    pBuyPriceNse = 0;
    pSellPriceNse = 0;
    pBuyPriceNse1 = 0;
    pSellPriceNse1 = 0;
    pBuySizeSize = 0;
    pSellSizeSize = 0;
    pBuySizeSize1 = 0;
    pSellSizeSize1 = 0;
    prevCount = 0;
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
} 	


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
    	DEBUG_MESSAGE(reqQryDebugLog(),"CustomResponseFailed");
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
    DEBUG_MESSAGE(reqQryDebugLog(), "onCMDModifyStrategy");
    API2::UserParams *params =(API2::UserParams *) newParams;	

    if(params->getValue("BuyTotalLot",BuyTotalLot) != API2::UserParamsError_OK || params->getValue("SellTotalLot",SellTotalLot) != API2::UserParamsError_OK)
    {
        DEBUG_MESSAGE(reqQryDebugLog(), "Error Either in BuyTotalLot OR SellTotalLot");
        ErrorFlag = 1;
        Update_Status("Error Either in BuyTotalLot OR SellTotalLot");
        DEBUG_FLUSH(reqQryDebugLog());
        reqTerminateStrategy();
    }
    else
    {
        if(BuyTotalLot - _TotalBuyTraded > 2000 || SellTotalLot - _TotalSellTraded > 2000)
        {
            DEBUG_MESSAGE(reqQryDebugLog(), "Open BuyTotal_Lot or SellTotal_Lot cant be more than 2000");
            ErrorFlag = 1;
            Update_Status("Error Either in BuyTotalLot - _TotalBuyTraded or SellTotalLot - _TotalSellTraded");
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

    if(_BuyTotalLot - First_Buy_Traded_Qty > 2000 || _SellTotalLot - First_Sell_Traded_Qty > 2000)
    {
        DEBUG_MESSAGE(reqQryDebugLog(), "Error Either in _BuyTotalLot / _SellTotalLot excess Quantity");
        ErrorFlag = 1;
        Update_Status("Error Either in _BuyTotalLot / _SellTotalLot excess Quantity");
        DEBUG_FLUSH(reqQryDebugLog());
        reqTerminateStrategy();
    }

    
    if(params->getValue("MaxOrderLot",MaxOrderLot) != API2::UserParamsError_OK)
    {
        DEBUG_MESSAGE(reqQryDebugLog(), "Error in MaxOrderLot");
         ErrorFlag = 1;
         Update_Status("Error in MaxOrderLot");
         DEBUG_FLUSH(reqQryDebugLog());
         reqTerminateStrategy();
    }
    else
    {
	    if(MaxOrderLot > 0 && MaxOrderLot <= 200)
	    {
	         _MaxOrderLot = MaxOrderLot;
	    }
	    else
	    {
	        DEBUG_MESSAGE(reqQryDebugLog(), "Error in MaxOrderLot");
	         ErrorFlag = 1;
	         Update_Status("Error in MaxOrderLot");
	         DEBUG_FLUSH(reqQryDebugLog());
	         reqTerminateStrategy();
	    }
    }

    if(params->getValue("BuyDiff",BuyDiff1) != API2::UserParamsError_OK || params->getValue("SellDiff",SellDiff1) != API2::UserParamsError_OK)
    {
        DEBUG_MESSAGE(reqQryDebugLog(), "Error Either in Difference entered into user Parameters.");
        ErrorFlag = 1;
        Update_Status("Error Either in Difference entered into user Parameters");
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
        DEBUG_MESSAGE(reqQryDebugLog(), "Error in IsBidding");
        ErrorFlag = 1;
        Update_Status("Error in IsBidding");
        DEBUG_FLUSH(reqQryDebugLog());
        reqTerminateStrategy();
    }
    else{
    	_IsBidding = IsBidding;
    }    
          

    if(params->getValue("StopLoss",StopLoss1) != API2::UserParamsError_OK || params->getValue("TriggerPrice",TriggerPrice1) != API2::UserParamsError_OK || params->getValue("Tolerance",Tolerance1) != API2::UserParamsError_OK)
    {
        DEBUG_MESSAGE(reqQryDebugLog(), "Error in Stoploss / TriggerPrice / Tolerance  entered into user Parameters.");
        ErrorFlag = 1;
        Update_Status("Error in Stoploss / TriggerPrice / Tolerance  entered into user Parameters");
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
        DEBUG_MESSAGE(reqQryDebugLog(), "Error in Depth");
        ErrorFlag = 1;
        Update_Status("Error in Depth");
        DEBUG_FLUSH(reqQryDebugLog());
        reqTerminateStrategy();
    }
    else{
        _Depth = Depth;
    }

    reqSendStrategyResponse(API2::CONSTANTS::RSP_ResponseType_STRATEGY_RUNNING,API2::CONSTANTS::RSP_RiskStatus_SUCCESS,API2::CONSTANTS::RSP_StrategyComment_USER_INPUT);
}

void CurrencyBseNse::onMarketDataEvent(UNSIGNED_LONG symbolId)
{
    DEBUG_VARSHOW(reqQryDebugLog(), "Market Data Event",symbolId);
    //DEBUG_FLUSH(reqQryDebugLog());
    API2::COMMON::MktData *mktDataNse = reqQryMarketData(_FeedInstrumentNse->getSymbolId());

    if(FeedCount < 5)
    {
        FeedCount = FeedCount + 1;
    }

    BuyPriceNse = mktDataNse->getBidPrice(0);
    SellPriceNse = mktDataNse->getAskPrice(0);
    BuyPriceNse1 = mktDataNse->getBidPrice(1);
    SellPriceNse1 = mktDataNse->getAskPrice(1);
    BuySizeSize = mktDataNse->getBidQty(0);
    SellSizeSize = mktDataNse->getAskQty(0);
    BuySizeSize1 = mktDataNse->getBidQty(1);
    SellSizeSize1 = mktDataNse->getAskQty(1);

    DEBUG_VARSHOW(reqQryDebugLog(), "buysize ",BuySizeSize);
    DEBUG_VARSHOW(reqQryDebugLog(), "sellsize ",SellSizeSize);
    DEBUG_VARSHOW(reqQryDebugLog(), "1buysize ",BuySizeSize1);
    DEBUG_VARSHOW(reqQryDebugLog(), "1sellsize ",SellSizeSize1);


    TickSize = _FeedInstrumentNse->getStaticData()->tickSize;
    Error = 0;


    if(_IsBidding.compare("Yes") != 0 || BuyPriceNse <= 0 || SellPriceNse <= 0 || BuySizeSize <= 0 || SellSizeSize <=0
       || BuyPriceNse - BuyPriceNse1 >= 6*TickSize ||  SellPriceNse1 - SellPriceNse >= 6*TickSize
       || SellPriceNse - BuyPriceNse >= 16*TickSize || ErrorFlag == 1)
    {
        //DEBUG_MESSAGE(reqQryDebugLog(), "Feed Error .... ");
        Error = 1;
    }  

    if(_Depth == 0) // first depth
    {
        BuyQuotePrice = BuyPriceNse - _BuyDiff;
        SellQuotePrice = SellPriceNse + _SellDiff;
        OrderBuySize = FindMinimumSize(BuySizeSize/2, _MaxOrderLot);
        OrderSellSize = FindMinimumSize(SellSizeSize/2, _MaxOrderLot);
    }

    if(_Depth == 1) // second depth
    {
        BuyQuotePrice = BuyPriceNse1 - _BuyDiff;
        SellQuotePrice = SellPriceNse1 + _SellDiff;
        OrderBuySize = FindMinimumSize(BuySizeSize1/2, _MaxOrderLot);
        OrderSellSize = FindMinimumSize(SellSizeSize1/2, _MaxOrderLot);
    }

    RemBuyQty = 0;
    RemSellQty = 0;

    First_Buy_Pending_Qty = _orderWrapperBuy._instrument->getPosition()->getPendingQty(API2::CONSTANTS::CMD_OrderMode_BUY)/BoardLotSizeBse;
    First_Sell_Pending_Qty = _orderWrapperSell._instrument->getPosition()->getPendingQty(API2::CONSTANTS::CMD_OrderMode_SELL)/BoardLotSizeBse;
    First_Leg_BuyOrder_Price = _orderWrapperBuy._instrument->getLastQuotedPrice(API2::CONSTANTS::CMD_OrderMode_BUY);
    First_Leg_SellOrder_Price = _orderWrapperSell._instrument->getLastQuotedPrice(API2::CONSTANTS::CMD_OrderMode_SELL); 

    DEBUG_VARSHOW(reqQryDebugLog(), " First leg buy pending : ",First_Buy_Pending_Qty);
    DEBUG_VARSHOW(reqQryDebugLog(), " First leg sell pending : ",First_Sell_Pending_Qty); 
    DEBUG_VARSHOW(reqQryDebugLog(), " First leg buy price : ",First_Leg_BuyOrder_Price);
    DEBUG_VARSHOW(reqQryDebugLog(), " First leg sell price : ",First_Leg_SellOrder_Price);  


    if(BuyQuotePrice - First_Leg_BuyOrder_Price >= 300 && First_Leg_BuyOrder_Price > 0 && BuyPriceNse > 0){
        BuyQuotePrice = First_Leg_BuyOrder_Price;
    }

    if(First_Leg_SellOrder_Price - SellQuotePrice >= 300 && First_Leg_SellOrder_Price > 0 && SellPriceNse > 0){
        SellQuotePrice = First_Leg_SellOrder_Price;
    }

    DEBUG_VARSHOW(reqQryDebugLog(), "Maximum order lot : ",_MaxOrderLot);
    DEBUG_VARSHOW(reqQryDebugLog(), "Quote for buy : ",BuyQuotePrice);
    DEBUG_VARSHOW(reqQryDebugLog(), "Quote for sell : ",SellQuotePrice); 

    First_leg_BuyTraded = _BseTradeInstr -> getPosition()-> getTradedQty(API2::CONSTANTS::CMD_OrderMode_BUY)/BoardLotSizeBse;
    Second_leg_SellTraded = _FeedInstrumentNse -> getPosition()-> getTradedQty(API2::CONSTANTS::CMD_OrderMode_SELL)/BoardLotSizeNse;
    Second_Sell_Pending_Qty = _FeedInstrumentNse->getPosition()->getPendingQty(API2::CONSTANTS::CMD_OrderMode_SELL)/BoardLotSizeNse;
    RemBuyQty = _BuyTotalLot - First_leg_BuyTraded;    
    OrderBuySize = FindMinimumSize(OrderBuySize, RemBuyQty);

    DEBUG_VARSHOW(reqQryDebugLog(), " Buy Order Size  : ",OrderBuySize); 
    DEBUG_VARSHOW(reqQryDebugLog(), " Buy Traded  : ",FirstLegBuyTradedQty); 

    if(_orderWrapperBuy._isPendingNew || _orderWrapperBuy._isPendingReplace || _orderWrapperBuy._isPendingCancel)
    {
        DEBUG_MESSAGE(reqQryDebugLog(), "First leg buy order is in pending state");
    }
    else
    {
    if(First_Buy_Pending_Qty > 0 && First_Leg_BuyOrder_Price > 0 && !_orderWrapperBuy._isReset)     // order is open
    {
            if(Error == 1 || ErrorFlag == 1)
            {
                _orderWrapperBuy.cancelOrder(risk);
                //DEBUG_VARSHOW(reqQryDebugLog(), " First Leg Buy Order cancelled at Price  : ",First_Leg_BuyOrder_Price);
            }
            else
            {                
                cond = 0;

                    if(cond == 0 && _MaxOrderLot > BuySizeSize)
                    {
                        if(First_Leg_BuyOrder_Price != BuyQuotePrice - TickSize && (BuyQuotePrice - TickSize) > 0)
                        {
                        	if(clock_gettime(CLOCK_REALTIME, &start) == -1){
				                perror("clock gettime");
				                exit( EXIT_FAILURE );
							}
                            _orderWrapperBuy.replaceOrder(risk, BuyQuotePrice - TickSize, (OrderBuySize*BoardLotSizeBse) + FirstLegBuyTradedQty);
                            if(clock_gettime(CLOCK_REALTIME, &end) == -1){
				                perror("clock gettime");
				                exit( EXIT_FAILURE );
							}
							double diff = (end.tv_sec*1000000000 + end.tv_nsec) - (start.tv_sec*1000000000 + start.tv_nsec);
							DEBUG_VARSHOW(reqQryDebugLog(), "case 1: Modify buy order send time: ",diff/1000);
                            cond = 1;
                            DEBUG_VARSHOW(reqQryDebugLog(), " case 1: Modify buy order at : ",BuyQuotePrice - TickSize);
                            DEBUG_VARSHOW(reqQryDebugLog(), " case 1: Modify buy order at : ",(OrderBuySize*BoardLotSizeBse) + FirstLegBuyTradedQty);
                        }
                    
                    }
                    if(cond == 0 && First_Leg_BuyOrder_Price != BuyQuotePrice && BuyQuotePrice > 0)
                    {
                    	if(clock_gettime(CLOCK_REALTIME, &start) == -1){
				                perror("clock gettime");
				                exit( EXIT_FAILURE );
						}
                        _orderWrapperBuy.replaceOrder(risk, BuyQuotePrice, (OrderBuySize*BoardLotSizeBse) + FirstLegBuyTradedQty);
                        if(clock_gettime(CLOCK_REALTIME, &end) == -1){
				                perror("clock gettime");
				                exit( EXIT_FAILURE );
						}
						double diff = (end.tv_sec*1000000000 + end.tv_nsec) - (start.tv_sec*1000000000 + start.tv_nsec);
						DEBUG_VARSHOW(reqQryDebugLog(), "case 2: Modify buy order send time: ",diff/1000);
                        DEBUG_VARSHOW(reqQryDebugLog(), " case 2: Modify buy order at : ",BuyQuotePrice);
                        DEBUG_VARSHOW(reqQryDebugLog(), " case 2: Modify buy order at : ",(OrderBuySize*BoardLotSizeBse) + FirstLegBuyTradedQty);
                        cond = 1;
                    }

                    if(cond == 0 && First_Buy_Pending_Qty != OrderBuySize){
                    	if(clock_gettime(CLOCK_REALTIME, &start) == -1){
				                perror("clock gettime");
				                exit( EXIT_FAILURE );
						}
                    	_orderWrapperBuy.replaceOrder(risk, 0, (OrderBuySize*BoardLotSizeBse) + FirstLegBuyTradedQty);
                        if(clock_gettime(CLOCK_REALTIME, &end) == -1){
				                perror("clock gettime");
				                exit( EXIT_FAILURE );
						}
						double diff = (end.tv_sec*1000000000 + end.tv_nsec) - (start.tv_sec*1000000000 + start.tv_nsec);
						DEBUG_VARSHOW(reqQryDebugLog(), "case 3: Modify buy order send time: ",diff/1000);
                        DEBUG_VARSHOW(reqQryDebugLog(), "case 3: Modify buy order at : ",(OrderBuySize*BoardLotSizeBse) + FirstLegBuyTradedQty);
                        cond = 1;
                    }
            }
    }    

    if(_orderWrapperBuy._isReset && Error == 0)		// 1 when no order
    {    
    if(BuyQuotePrice > 0 && FeedCount > 3 && First_leg_BuyTraded - Second_leg_SellTraded == 0 && First_Buy_Pending_Qty == 0 && Second_Sell_Pending_Qty == 0 && _BuyTotalLot >= First_leg_BuyTraded + OrderBuySize + _TotalBuyTraded)
    {
    	if(clock_gettime(CLOCK_REALTIME, &start) == -1){
                perror("clock gettime");
                exit( EXIT_FAILURE );
		}
        _orderWrapperBuy.newOrder(risk, BuyQuotePrice,OrderBuySize*BoardLotSizeBse);
        if(clock_gettime(CLOCK_REALTIME, &end) == -1){
                perror("clock gettime");
                exit( EXIT_FAILURE );
		}
		double diff = (end.tv_sec*1000000000 + end.tv_nsec) - (start.tv_sec*1000000000 + start.tv_nsec);
		DEBUG_VARSHOW(reqQryDebugLog(), "New Buy Order send time: ",diff/1000);
        DEBUG_VARSHOW(reqQryDebugLog(), " New Buy Order placed at : ",BuyQuotePrice);
        DEBUG_VARSHOW(reqQryDebugLog(), " New Buy Order placed at : ",OrderBuySize*BoardLotSizeBse);
        OrderIdOrderWrapperSellMap.clear();
        OrderIdOrderWrapperSellMapSelfTrade.clear();
        FirstLegBuyTradePrice = 0;
        FirstLegBuyTradedQty = 0;
    }
    }
    }

    First_leg_SellTraded = _BseTradeInstr -> getPosition()-> getTradedQty(API2::CONSTANTS::CMD_OrderMode_SELL)/BoardLotSizeBse;
    Second_leg_BuyTraded = _FeedInstrumentNse -> getPosition()-> getTradedQty(API2::CONSTANTS::CMD_OrderMode_BUY)/BoardLotSizeNse;
    Second_Buy_Pending_Qty = _FeedInstrumentNse->getPosition()->getPendingQty(API2::CONSTANTS::CMD_OrderMode_BUY)/BoardLotSizeNse;
    RemSellQty = _SellTotalLot - First_leg_SellTraded;    
    OrderSellSize = FindMinimumSize(OrderSellSize, RemSellQty);  
    DEBUG_VARSHOW(reqQryDebugLog(), " Sell Order Size  : ",OrderSellSize); 
    DEBUG_VARSHOW(reqQryDebugLog(), " Sell Traded  : ",FirstLegSellTradedQty); 

    if(_orderWrapperSell._isPendingNew || _orderWrapperSell._isPendingReplace || _orderWrapperSell._isPendingCancel)
    {
    	DEBUG_MESSAGE(reqQryDebugLog(), "First leg sell order is in pending state");
    }
    else
    {
    if(First_Sell_Pending_Qty > 0 && First_Leg_SellOrder_Price > 0 && !_orderWrapperSell._isReset)
    {

        if(Error == 1 || ErrorFlag == 1)
        {
            _orderWrapperSell.cancelOrder(risk);
            //DEBUG_VARSHOW(reqQryDebugLog(), " First Leg Sell Order cancelled at Price  : ",First_Leg_SellOrder_Price);
        }
        else
        {
            cond = 0;

            if(cond == 0 && _MaxOrderLot > SellSizeSize)
            {
                if(First_Leg_SellOrder_Price != SellQuotePrice + TickSize && (SellQuotePrice + TickSize) > 0)
                {
                	if(clock_gettime(CLOCK_REALTIME, &start) == -1){
			                perror("clock gettime");
			                exit( EXIT_FAILURE );
					}
                    _orderWrapperSell.replaceOrder(risk,SellQuotePrice + TickSize,(OrderSellSize*BoardLotSizeBse)+ FirstLegSellTradedQty);
                    if(clock_gettime(CLOCK_REALTIME, &end) == -1){
		                perror("clock gettime");
		                exit( EXIT_FAILURE );
					}
					double diff = (end.tv_sec*1000000000 + end.tv_nsec) - (start.tv_sec*1000000000 + start.tv_nsec);
					DEBUG_VARSHOW(reqQryDebugLog(), "case 1: Modify Sell order send time: ",diff/1000);
                    cond = 1;
                    DEBUG_VARSHOW(reqQryDebugLog(), "case 1: Modify Sell order at : ",SellQuotePrice + TickSize);
                    DEBUG_VARSHOW(reqQryDebugLog(), "case 1: Modify Sell order at : ",(OrderSellSize*BoardLotSizeBse)+ FirstLegSellTradedQty);
                }
            }

            if(cond == 0 && First_Leg_SellOrder_Price != SellQuotePrice && SellQuotePrice > 0)
            {
            	if(clock_gettime(CLOCK_REALTIME, &start) == -1){
		                perror("clock gettime");
		                exit( EXIT_FAILURE );
				}
             _orderWrapperSell.replaceOrder(risk,SellQuotePrice,(OrderSellSize*BoardLotSizeBse)+ FirstLegSellTradedQty);
             	if(clock_gettime(CLOCK_REALTIME, &end) == -1){
	                perror("clock gettime");
	                exit( EXIT_FAILURE );
				}
				double diff = (end.tv_sec*1000000000 + end.tv_nsec) - (start.tv_sec*1000000000 + start.tv_nsec);
				DEBUG_VARSHOW(reqQryDebugLog(), "case 2: Modify Sell order send time: ",diff/1000);
             DEBUG_VARSHOW(reqQryDebugLog(), "case 2: Modify Sell order at : ",SellQuotePrice);
             DEBUG_VARSHOW(reqQryDebugLog(), "case 2: Modify Sell order at : ",(OrderSellSize*BoardLotSizeBse)+ FirstLegSellTradedQty);
             cond = 1;
            } 

            if(cond == 0 && First_Sell_Pending_Qty != OrderSellSize) {
            	if(clock_gettime(CLOCK_REALTIME, &start) == -1){
		                perror("clock gettime");
		                exit( EXIT_FAILURE );
				}
        	 _orderWrapperSell.replaceOrder(risk,0,(OrderSellSize*BoardLotSizeBse)+ FirstLegSellTradedQty);
        	    if(clock_gettime(CLOCK_REALTIME, &end) == -1){
	                perror("clock gettime");
	                exit( EXIT_FAILURE );
				}
			double diff = (end.tv_sec*1000000000 + end.tv_nsec) - (start.tv_sec*1000000000 + start.tv_nsec);
			DEBUG_VARSHOW(reqQryDebugLog(), "case 3: Modify Sell order send time: ",diff/1000);
             DEBUG_VARSHOW(reqQryDebugLog(), "case 3: Modify Sell order at : ",(OrderSellSize*BoardLotSizeBse)+ FirstLegSellTradedQty);
             cond = 1;
            }         
        }
    }    


    if(_orderWrapperSell._isReset && Error == 0)		// 1 when no order
    {

    if(SellQuotePrice > 0 && FeedCount > 3 && Second_leg_BuyTraded - First_leg_SellTraded == 0 && First_Sell_Pending_Qty == 0 && Second_Buy_Pending_Qty == 0 && _SellTotalLot >= First_leg_SellTraded + OrderSellSize + _TotalSellTraded)
    {
    	if(clock_gettime(CLOCK_REALTIME, &start) == -1){
                perror("clock gettime");
                exit( EXIT_FAILURE );
		}
    _orderWrapperSell.newOrder(risk,SellQuotePrice,OrderSellSize*BoardLotSizeBse);
    if(clock_gettime(CLOCK_REALTIME, &end) == -1){
        perror("clock gettime");
        exit( EXIT_FAILURE );
	}
	double diff = (end.tv_sec*1000000000 + end.tv_nsec) - (start.tv_sec*1000000000 + start.tv_nsec);
	DEBUG_VARSHOW(reqQryDebugLog(), "New Sell order send time: ",diff/1000);
    DEBUG_VARSHOW(reqQryDebugLog(), " New Sell Order placed at : ",SellQuotePrice);
    DEBUG_VARSHOW(reqQryDebugLog(), " New Sell Order placed at : ",OrderSellSize*BoardLotSizeBse);
    OrderIdOrderWrapperBuyMap.clear();
    OrderIdOrderWrapperBuyMapSelfTrade.clear();
    FirstLegSellTradePrice = 0;
    FirstLegSellTradedQty = 0;
    }
    }
    }

    if(Second_Sell_Pending_Qty > 0)
    {
        for(auto iter = OrderIdOrderWrapperSellMap.begin(); iter != OrderIdOrderWrapperSellMap.end(); ++iter)
            {
                OrderPriceSellSecond = iter->second->_instrument->getLastQuotedPrice(API2::CONSTANTS::CMD_OrderMode_SELL);
                dTrigger = OrderPriceSellSecond - _TriggerPrice;
                // DEBUG_VARSHOW(reqQryDebugLog(), " OrderPriceSellSecond : ",OrderPriceSellSecond);
                // DEBUG_VARSHOW(reqQryDebugLog(), " dTrigger : ",dTrigger);
                // DEBUG_VARSHOW(reqQryDebugLog(), " SellPriceNse : ",SellPriceNse);
                 if(SellPriceNse <= dTrigger && OrderPriceSellSecond > 0){
                 	iter->second->replaceOrder(risk,dTrigger - _StopLoss,0);
                 	//DEBUG_VARSHOW(reqQryDebugLog(), " Modify at : ",dTrigger - _StopLoss);
                 }                 
                    
            }

        for(auto iter = OrderIdOrderWrapperSellMapSelfTrade.begin(); iter != OrderIdOrderWrapperSellMapSelfTrade.end(); ++iter)
            {
                OrderPriceSellSecond = iter->second->_instrument->getLastQuotedPrice(API2::CONSTANTS::CMD_OrderMode_SELL);
                dTrigger = OrderPriceSellSecond - _TriggerPrice;
                // DEBUG_VARSHOW(reqQryDebugLog(), " OrderPriceSellSecond : ",OrderPriceSellSecond);
                // DEBUG_VARSHOW(reqQryDebugLog(), " dTrigger : ",dTrigger);
                // DEBUG_VARSHOW(reqQryDebugLog(), " SellPriceNse : ",SellPriceNse);
                iCond = 0;
                if(SellPriceNse <= dTrigger && OrderPriceSellSecond > 0)
                {
                	iter->second->replaceOrder(risk,dTrigger - _StopLoss,0);
                	//DEBUG_VARSHOW(reqQryDebugLog(), "1 Modify at : ",dTrigger - _StopLoss);
                	iCond = 1;                
                }


                // DEBUG_VARSHOW(reqQryDebugLog(), " BuyPriceNse : ",BuyPriceNse);
                // DEBUG_VARSHOW(reqQryDebugLog(), " SecLegSellOrderP : ",SecLegSellOrderP);
                if(iCond == 0 && BuyPriceNse >= SecLegSellOrderP && OrderPriceSellSecond != SecLegSellOrderP)
                {
                     iter->second->replaceOrder(risk,SecLegSellOrderP, 0);
                     //DEBUG_VARSHOW(reqQryDebugLog(), "2 Modify at : ",SecLegSellOrderP);
                     iCond = 1;
                }

                 if(iCond == 0 && BuyPriceNse < SecLegSellOrderP)
                 {
                     if(OrderPriceSellSecond - BuyPriceNse > TickSize)
                     {
                         NewPrice = OrderPriceSellSecond - TickSize;
                         //DEBUG_VARSHOW(reqQryDebugLog(), "NewPrice: ",NewPrice);
                         if(NewPrice >= SecLegSellOrderP)
                         {
                             iter->second->replaceOrder(risk,NewPrice, 0);
                             //DEBUG_VARSHOW(reqQryDebugLog(), "3 Modify at : ",NewPrice);
                             iCond = 1;
                         }
                     }
                 }
            }
            //DEBUG_FLUSH(reqQryDebugLog());
    }

    if(Second_Buy_Pending_Qty > 0)
    {
        for(auto iter = OrderIdOrderWrapperBuyMap.begin(); iter != OrderIdOrderWrapperBuyMap.end(); ++iter)
            {
                OrderPriceBuySecond = iter->second->_instrument->getLastQuotedPrice(API2::CONSTANTS::CMD_OrderMode_BUY);
                dTrigger = OrderPriceBuySecond + _TriggerPrice;   
                // DEBUG_VARSHOW(reqQryDebugLog(), " OrderPriceBuySecond : ",OrderPriceBuySecond);
                // DEBUG_VARSHOW(reqQryDebugLog(), " dTrigger : ",dTrigger);
                // DEBUG_VARSHOW(reqQryDebugLog(), " BuyPriceNse : ",BuyPriceNse);             
                if(BuyPriceNse >= dTrigger && OrderPriceBuySecond > 0){
                	iter->second->replaceOrder(risk,dTrigger + _StopLoss, 0);
                	//DEBUG_VARSHOW(reqQryDebugLog(), " Modify at : ",dTrigger + _StopLoss);
                }               
                    
            }

        for(auto iter = OrderIdOrderWrapperBuyMapSelfTrade.begin(); iter != OrderIdOrderWrapperBuyMapSelfTrade.end(); ++iter)
            {
                OrderPriceBuySecond = iter->second->_instrument->getLastQuotedPrice(API2::CONSTANTS::CMD_OrderMode_BUY);
                dTrigger = OrderPriceBuySecond + _TriggerPrice;
                // DEBUG_VARSHOW(reqQryDebugLog(), " OrderPriceBuySecond : ",OrderPriceBuySecond);
                // DEBUG_VARSHOW(reqQryDebugLog(), " dTrigger : ",dTrigger);
                // DEBUG_VARSHOW(reqQryDebugLog(), " BuyPriceNse : ",BuyPriceNse); 
                iCond = 0;
                if(BuyPriceNse >= dTrigger && OrderPriceBuySecond > 0)
                {
                iter->second->replaceOrder(risk,dTrigger + _StopLoss, 0);
                //DEBUG_VARSHOW(reqQryDebugLog(), "4 Modify at : ",dTrigger + _StopLoss);
                iCond = 1;
                }


                // DEBUG_VARSHOW(reqQryDebugLog(), " SellPriceNse : ",SellPriceNse);
                // DEBUG_VARSHOW(reqQryDebugLog(), " SecLegBuyOrderP : ",SecLegBuyOrderP);
                if(iCond == 0 && SellPriceNse <= SecLegBuyOrderP && OrderPriceBuySecond != SecLegBuyOrderP)
                {
                    iter->second->replaceOrder(risk,SecLegBuyOrderP, 0);
                    //DEBUG_VARSHOW(reqQryDebugLog(), "5 Modify at : ",SecLegBuyOrderP);
                    iCond = 1;
                }

                if(iCond == 0 && SellPriceNse > SecLegBuyOrderP)
                {
                    if(SellPriceNse - OrderPriceBuySecond > TickSize)
                    {
                        NewPrice = OrderPriceBuySecond + TickSize;
                        //DEBUG_VARSHOW(reqQryDebugLog(), "NewPrice: ",NewPrice);
                        if(NewPrice <= SecLegBuyOrderP)
                        {
                            iter->second->replaceOrder(risk,NewPrice, 0);
                            //DEBUG_VARSHOW(reqQryDebugLog(), "3 Modify at : ",NewPrice);
                            iCond = 1;
                        }
                    }
                }
            }
            //DEBUG_FLUSH(reqQryDebugLog());
    }
    DEBUG_FLUSH(reqQryDebugLog());
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
    _BuySide = API2::CONSTANTS::CMD_OrderMode_BUY;
    _SellSide = API2::CONSTANTS::CMD_OrderMode_SELL;

    SecLegBuyOrderP = 0;
    SecLegSellOrderP = 0;

    if(params->getValue("Acc Detail 1",accountDetailLeg1)!=API2::UserParamsError_OK)
    {
        DEBUG_MESSAGE(reqQryDebugLog(), "Error in FirstLegAccount");
        DEBUG_FLUSH(reqQryDebugLog());
        ParamError = 1;
        return false ;
    }
    if(params->getValue("Acc Detail 2",accountDetailLeg2)!=API2::UserParamsError_OK)
    {
        DEBUG_MESSAGE(reqQryDebugLog(), "Error in SecondLegAccount");
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
        DEBUG_MESSAGE(reqQryDebugLog(), "Error in TotalBuyTraded OR TotalSellTraded");
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
        DEBUG_MESSAGE(reqQryDebugLog(), "Error in BuyTotal_Lot OR SellTotal_Lot");
        DEBUG_FLUSH(reqQryDebugLog());
        ParamError = 1;
        return false;
	}
	else
	{
        if(BuyTotalLot - _TotalBuyTraded > 2000 || SellTotalLot - _TotalSellTraded > 2000)
        {
            DEBUG_MESSAGE(reqQryDebugLog(), "Open BuyTotal_Lot OR SellTotal_Lot cant be more than 2000");
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
        DEBUG_MESSAGE(reqQryDebugLog(), "Error in Order_Lot");
        DEBUG_FLUSH(reqQryDebugLog());
        ParamError = 1;
        return false;
	}
	else
	{
	    if(MaxOrderLot > 0 && MaxOrderLot <= 200){
	    	_MaxOrderLot = MaxOrderLot;
	    }    
	    else
	    {
	        DEBUG_MESSAGE(reqQryDebugLog(), "Error in MaxOrderLot entered into user Parameters.");
	        DEBUG_FLUSH(reqQryDebugLog());
	        ParamError = 1;
	        return false;
	    }
	}  

    //DEBUG_VARSHOW(reqQryDebugLog(), " _MaxOrderLot : ",_MaxOrderLot);

    if(params->getValue("StopLoss",StopLoss1) != API2::UserParamsError_OK || params->getValue("TriggerPrice",TriggerPrice1) != API2::UserParamsError_OK || params->getValue("Tolerance",Tolerance1) != API2::UserParamsError_OK)
    {
        DEBUG_MESSAGE(reqQryDebugLog(), "Error in Stoploss / TriggerPrice / Tolerance  entered into user Parameters.");
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
        DEBUG_MESSAGE(reqQryDebugLog(), "Error in Difference entered into user Parameters.");
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
        DEBUG_MESSAGE(reqQryDebugLog(), "Error in IsBidding");
        DEBUG_FLUSH(reqQryDebugLog());
        ParamError = 1;
        return false;
    }
    else
    {
        _IsBidding = IsBidding;
    }

    if(params->getValue("Depth",Depth) != API2::UserParamsError_OK)
    {
        DEBUG_MESSAGE(reqQryDebugLog(), "Error in Depth ");
        DEBUG_FLUSH(reqQryDebugLog());
        ParamError = 1;
        return false;
    }
    else{
        _Depth = Depth;                           // First = 0 or Second = 1
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

    _orderWrapperBuy = API2_NEW::COMMON::OrderWrapper(_BseTradeInstr, _BuySide, this, AccountDetailBse);
    _orderWrapperSell = API2_NEW::COMMON::OrderWrapper(_BseTradeInstr, _SellSide, this, AccountDetailBse);

     DEBUG_FLUSH(reqQryDebugLog());
     return true;
}

void CurrencyBseNse::onCanceled(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId)
{
    //DEBUG_MESSAGE(reqQryDebugLog(), "onCanceled");


    TickSize = _FeedInstrumentNse->getStaticData()->tickSize;
    if(_orderWrapperBuy._orderId == orderId)
    {
     _orderWrapperBuy._isPendingCancel = false;
     _orderWrapperBuy._isPendingNew = false;
     _orderWrapperBuy._isPendingReplace = false;
     _orderWrapperBuy.reset();
    }

    if(_orderWrapperSell._orderId == orderId)
    {
         _orderWrapperSell._isPendingCancel = false;
         _orderWrapperSell._isPendingNew = false;
         _orderWrapperSell._isPendingReplace = false;
         _orderWrapperSell.reset();        
    }

    auto iter2 = OrderIdOrderWrapperBuyMap.find(orderId);
        if(iter2 != OrderIdOrderWrapperBuyMap.end())
            {
                SecLegBuyOrderP = iter2->second->getOrderPrice() - TickSize;
                API2::SingleOrder* order = reqQryOrder(confirmation.getClOrderId());
                         SIGNED_LONG CancelQty = 0;

                     if( order )
                        CancelQty = order->getQuantity() - order->getFilledQuantity();

                if(SecLegBuyOrderP > 0)
                {
                API2_NEW::COMMON::OrderWrapper *_orderWrapperSecondBuy = new API2_NEW::COMMON::OrderWrapper(_FeedInstrumentNse,_BuySide,this,AccountDetailNse);
                // API2::COMMON::OrderWrapper *_orderWrapperSecondBuy = new API2::COMMON::OrderWrapper(_FeedInstrumentNse,_BuySide,this,AccountDetailNse);
                _orderWrapperSecondBuy->reset();
                OrderIdOrderWrapperBuyMapSelfTrade[_orderWrapperSecondBuy->_orderId]= _orderWrapperSecondBuy;
                _orderWrapperSecondBuy->newOrder(risk,SecLegBuyOrderP ,CancelQty);
                OrderIdOrderWrapperBuyMapSelfTrade.insert(std::pair<API2::COMMON::OrderId*,API2_NEW::COMMON::OrderWrapper*>(_orderWrapperSecondBuy->_orderId,_orderWrapperSecondBuy));

                }
            }

    auto iter3 = OrderIdOrderWrapperSellMap.find(orderId);

        if(iter3 != OrderIdOrderWrapperSellMap.end())
            {
                SecLegSellOrderP = iter3->second->getOrderPrice() + TickSize;
                API2::SingleOrder* order = reqQryOrder(confirmation.getClOrderId());
                         SIGNED_LONG CancelQty = 0;

                     if( order )
                        CancelQty = order->getQuantity() - order->getFilledQuantity();

                if(SecLegSellOrderP > 0)
                {
                    API2_NEW::COMMON::OrderWrapper *_orderWrapperSecondSell = new API2_NEW::COMMON::OrderWrapper(_FeedInstrumentNse,_SellSide,this,AccountDetailNse);
                    //API2::COMMON::OrderWrapper *_orderWrapperSecondSell = new API2::COMMON::OrderWrapper(_FeedInstrumentNse,_SellSide,this,AccountDetailNse);
                    _orderWrapperSecondSell->reset();
                    OrderIdOrderWrapperSellMapSelfTrade[_orderWrapperSecondSell->_orderId]= _orderWrapperSecondSell;
                    _orderWrapperSecondSell->newOrder(risk,SecLegSellOrderP ,CancelQty);
                    OrderIdOrderWrapperSellMapSelfTrade.insert(std::pair<API2::COMMON::OrderId*,API2_NEW::COMMON::OrderWrapper*>(_orderWrapperSecondSell->_orderId,_orderWrapperSecondSell));

                }
            }
     //DEBUG_FLUSH(reqQryDebugLog());
}

void CurrencyBseNse::onReplaced(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId)
{
    //DEBUG_MESSAGE(reqQryDebugLog(), "onReplaced");

    //DEBUG_FLUSH(reqQryDebugLog());

    if(_orderWrapperBuy._orderId == orderId)
    {
    	DEBUG_MESSAGE(reqQryDebugLog(), "First leg buy modify order confirmation");
    if(!_orderWrapperBuy.processConfirmation(confirmation))
    {
    // DEBUG_MESSAGE(reqQryDebugLog(),"Order Wrapper Process Confirmation Failed _orderWrapperBuy");
    _orderWrapperBuy._isPendingCancel = false;
    _orderWrapperBuy._isPendingNew = false;
    _orderWrapperBuy._isPendingReplace = false;
    }
    }

    if(_orderWrapperSell._orderId == orderId)
    {
    	DEBUG_MESSAGE(reqQryDebugLog(), "First leg sell modify order confirmation");
    if(!_orderWrapperSell.processConfirmation(confirmation))
    {
    // DEBUG_MESSAGE(reqQryDebugLog(),"Order Wrapper Process Confirmation Failed _orderWrapperSell");
    _orderWrapperSell._isPendingCancel = false;
    _orderWrapperSell._isPendingNew = false;
    _orderWrapperSell._isPendingReplace = false;
    }
    }
    // DEBUG_MESSAGE(reqQryDebugLog(), "------------ onReplaced ends");
}

void CurrencyBseNse::onConfirmed(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId)
{
    if(_orderWrapperBuy._orderId == orderId)
    { 
    	DEBUG_MESSAGE(reqQryDebugLog(), "First leg buy new order confirmation");      
    if(!_orderWrapperBuy.processConfirmation(confirmation))
    {
     DEBUG_MESSAGE(reqQryDebugLog(),"Order Wrapper Process Confirmation Failed _orderWrapperBuy");
    }
    }


    if(_orderWrapperSell._orderId == orderId)
    {
    	DEBUG_MESSAGE(reqQryDebugLog(), "First leg sell new order confirmation");
    if(!_orderWrapperSell.processConfirmation(confirmation))
    {
     DEBUG_MESSAGE(reqQryDebugLog(),"Order Wrapper Process Confirmation Failed _orderWrapperSell");
    }
    }

     // DEBUG_MESSAGE(reqQryDebugLog(), "------------ onConfirmed ends");
}

void CurrencyBseNse::onFilled(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId)
{
    DEBUG_MESSAGE(reqQryDebugLog(), "onFilled Event");
    // DEBUG_FLUSH(reqQryDebugLog());
    API2::DATA_TYPES::RiskStatus risk;

    if(API2::DATA_TYPES::SYMBOL_ID(_BseTradeInstr->getSymbolId()) == confirmation.getSymbolId())
    {
    TrdQty = (confirmation.getLastFillQuantity()/BoardLotSizeBse)*BoardLotSizeNse;
    TrdPrice = confirmation.getLastFillPrice();

    //DEBUG_VARSHOW(reqQryDebugLog(), "TrdQty @: ",TrdQty);
    //DEBUG_VARSHOW(reqQryDebugLog(), "TrdPrice @: ",TrdPrice);

    if(confirmation.getOrderMode() == API2::CONSTANTS::CMD_OrderMode_BUY)
    {
    //DEBUG_MESSAGE(reqQryDebugLog(), "--------------- Trade Event Starts for 1st Leg BUY  ----------------");

     API2_NEW::COMMON::OrderWrapper *_orderWrapperSecondSell = new API2_NEW::COMMON::OrderWrapper(_FeedInstrumentNse,_SellSide,this,AccountDetailNse);
    _orderWrapperSecondSell->reset();
    OrderIdOrderWrapperSellMap[_orderWrapperSecondSell->_orderId]= _orderWrapperSecondSell;
    _orderWrapperSecondSell->newOrder(risk,TrdPrice + _BuyDiff ,TrdQty);
    DEBUG_VARSHOW(reqQryDebugLog(), " First leg buy traded at : ",TrdPrice);
    DEBUG_VARSHOW(reqQryDebugLog(), " First leg buy traded qty : ",confirmation.getLastFillQuantity()/BoardLotSizeBse);       
    DEBUG_VARSHOW(reqQryDebugLog(), " Second leg sell order placed at : ",TrdPrice + _BuyDiff);
    OrderIdOrderWrapperSellMap.insert(std::pair<API2::COMMON::OrderId*,API2_NEW::COMMON::OrderWrapper*>(_orderWrapperSecondSell->_orderId,_orderWrapperSecondSell));
    FirstLegBuyTradePrice = TrdPrice;
    FirstLegBuyTradedQty +=  confirmation.getLastFillQuantity(); 
    }

    if(confirmation.getOrderMode() == API2::CONSTANTS::CMD_OrderMode_SELL)
    {
       //DEBUG_MESSAGE(reqQryDebugLog(), "--------------- Trade Event Starts for 1st Leg Sell ----------------");

        API2_NEW::COMMON::OrderWrapper *_orderWrapperSecondBuy = new API2_NEW::COMMON::OrderWrapper(_FeedInstrumentNse,_BuySide,this,AccountDetailNse);       
        _orderWrapperSecondBuy->reset();
        OrderIdOrderWrapperBuyMap[_orderWrapperSecondBuy->_orderId]= _orderWrapperSecondBuy;
        _orderWrapperSecondBuy->newOrder(risk,TrdPrice - _SellDiff ,TrdQty);
        DEBUG_VARSHOW(reqQryDebugLog(), " First leg sell traded at : ",TrdPrice);
        DEBUG_VARSHOW(reqQryDebugLog(), " First leg sell traded qty : ",confirmation.getLastFillQuantity()/BoardLotSizeBse);       
        DEBUG_VARSHOW(reqQryDebugLog(), " Second leg buy order placed at : ",TrdPrice - _SellDiff);
        OrderIdOrderWrapperBuyMap.insert(std::pair<API2::COMMON::OrderId*,API2_NEW::COMMON::OrderWrapper*>(_orderWrapperSecondBuy->_orderId,_orderWrapperSecondBuy));
        FirstLegSellTradePrice = TrdPrice;
        FirstLegSellTradedQty +=  confirmation.getLastFillQuantity(); 
    }
	}


    if(API2::DATA_TYPES::SYMBOL_ID(_FeedInstrumentNse->getSymbolId()) == confirmation.getSymbolId())
	{

    TrdPrice = confirmation.getLastFillPrice();
    TrdQty = confirmation.getLastFillQuantity();

	if(confirmation.getOrderMode() == API2::CONSTANTS::CMD_OrderMode_BUY)
    {
        //DEBUG_MESSAGE(reqQryDebugLog(), "--------------- Trade Event Starts for 2nd Leg Nse Buy ----------------");

        First_Sell_Traded_Price = FirstLegSellTradePrice;
        Second_Buy_Traded_Price = TrdPrice;
        dTradeDiff = First_Sell_Traded_Price - Second_Buy_Traded_Price;

        if(dTradeDiff < (_SellDiff - _Tolerance))
        {
          //DEBUG_VARSHOW(reqQryDebugLog(), "_IsBidding  : ",_IsBidding);
         _IsBidding = "No";
         Update_Text(1);
          //DEBUG_VARSHOW(reqQryDebugLog(), "_IsBidding  : ",_IsBidding);
        }       

        First_Sell_Pending_Qty = _orderWrapperSell._instrument->getPosition()->getPendingQty(API2::CONSTANTS::CMD_OrderMode_SELL)/BoardLotSizeBse;
        First_leg_Sell_Position = _BseTradeInstr -> getPosition()-> getTradedQty(API2::CONSTANTS::CMD_OrderMode_SELL)/BoardLotSizeBse;
        Second_leg_Buy_Position = _FeedInstrumentNse -> getPosition()-> getTradedQty(API2::CONSTANTS::CMD_OrderMode_BUY)/BoardLotSizeNse;
        //DEBUG_VARSHOW(reqQryDebugLog(), "First_leg_Sell_Position  : ",First_leg_Sell_Position);
        //DEBUG_VARSHOW(reqQryDebugLog(), "Second_leg_Buy_Position  : ",Second_leg_Buy_Position);
        //DEBUG_VARSHOW(reqQryDebugLog(), "_TotalSellTraded  : ",_TotalSellTraded);

            // Update_Sell_Position(First_leg_Sell_Position + _TotalSellTraded);
        Update_Sell_Position(First_leg_Sell_Position);

        if(First_leg_Sell_Position == Second_leg_Buy_Position)
        {
           OrderIdOrderWrapperBuyMap.clear();
           OrderIdOrderWrapperBuyMapSelfTrade.clear();
            //DEBUG_MESSAGE(reqQryDebugLog(), "OrderIdOrderWrapperBuyMap clear : ");
            // DEBUG_FLUSH(reqQryDebugLog());
        }

        if(First_Sell_Pending_Qty <= 0){
            _orderWrapperSell._isPendingCancel = false;
           _orderWrapperSell._isPendingNew = false;
           _orderWrapperSell._isPendingReplace = false;
           _orderWrapperSell.reset();
        }

        // new changes
        onMarketDataEvent(confirmation.getSymbolId());
        DEBUG_FLUSH(reqQryDebugLog());
    }

	if(confirmation.getOrderMode() == API2::CONSTANTS::CMD_OrderMode_SELL)
    {
        //DEBUG_MESSAGE(reqQryDebugLog(), "--------------- Trade Event Starts for 2nd Leg Nse Sell ----------------");

        First_Buy_Traded_Price = FirstLegBuyTradePrice ;
        Second_Sell_Traded_Price = TrdPrice;
        dTradeDiff = Second_Sell_Traded_Price - First_Buy_Traded_Price;

        if(dTradeDiff < (_BuyDiff - _Tolerance))
        {
         //DEBUG_VARSHOW(reqQryDebugLog(), "_IsBidding  : ",_IsBidding);
        _IsBidding = "No";
         Update_Text(1);
         //DEBUG_VARSHOW(reqQryDebugLog(), "_IsBidding  : ",_IsBidding);
        }

        First_Buy_Pending_Qty = _orderWrapperBuy._instrument->getPosition()->getPendingQty(API2::CONSTANTS::CMD_OrderMode_BUY)/BoardLotSizeBse;
        First_leg_Buy_Position = _BseTradeInstr -> getPosition()-> getTradedQty(API2::CONSTANTS::CMD_OrderMode_BUY)/BoardLotSizeBse;
        Second_leg_Sell_Position = _FeedInstrumentNse -> getPosition()-> getTradedQty(API2::CONSTANTS::CMD_OrderMode_SELL)/BoardLotSizeNse;

        //DEBUG_VARSHOW(reqQryDebugLog(), "First_leg_Buy_Position  : ",First_leg_Buy_Position);
        //DEBUG_VARSHOW(reqQryDebugLog(), "Second_leg_Sell_Position  : ",Second_leg_Sell_Position);
        //DEBUG_VARSHOW(reqQryDebugLog(), "_TotalBuyTraded  : ",_TotalBuyTraded);        
        Update_Buy_Position(First_leg_Buy_Position);

        if(First_leg_Buy_Position == Second_leg_Sell_Position)
        {
            OrderIdOrderWrapperSellMap.clear();
            OrderIdOrderWrapperSellMapSelfTrade.clear();
            //DEBUG_MESSAGE(reqQryDebugLog(), "OrderIdOrderWrapperSellMap clear : ");            
        }

        if(First_Buy_Pending_Qty <= 0){
            _orderWrapperBuy._isPendingCancel = false;
            _orderWrapperBuy._isPendingNew = false;
            _orderWrapperBuy._isPendingReplace = false;
            _orderWrapperBuy.reset();
        }

        // new changes
        onMarketDataEvent(confirmation.getSymbolId());
        DEBUG_FLUSH(reqQryDebugLog());
    }
    }
    // DEBUG_MESSAGE(reqQryDebugLog(), "--------------- Trade Event Ends ----------------");
}

void CurrencyBseNse::onPartialFill(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId)
{
    DEBUG_MESSAGE(reqQryDebugLog(), "onPartialFill Event");
    //DEBUG_FLUSH(reqQryDebugLog());

    API2::DATA_TYPES::RiskStatus risk;

    if(API2::DATA_TYPES::SYMBOL_ID(_BseTradeInstr->getSymbolId()) == confirmation.getSymbolId())
    {
    TrdQty = (confirmation.getLastFillQuantity()/BoardLotSizeBse)*BoardLotSizeNse;
    TrdPrice = confirmation.getLastFillPrice();

    if(confirmation.getOrderMode() == API2::CONSTANTS::CMD_OrderMode_BUY)
    {
        //DEBUG_MESSAGE(reqQryDebugLog(), "--------------- Trade Event Starts for 1st Leg BUY  ----------------");
        API2_NEW::COMMON::OrderWrapper *_orderWrapperSecondSell = new API2_NEW::COMMON::OrderWrapper(_FeedInstrumentNse,_SellSide,this,AccountDetailNse);
        _orderWrapperSecondSell->reset();
        OrderIdOrderWrapperSellMap[_orderWrapperSecondSell->_orderId]= _orderWrapperSecondSell;
        _orderWrapperSecondSell->newOrder(risk,TrdPrice + _BuyDiff ,TrdQty); 
        DEBUG_VARSHOW(reqQryDebugLog(), " First leg buy traded at : ",TrdPrice);
        DEBUG_VARSHOW(reqQryDebugLog(), " First leg buy traded qty : ",confirmation.getLastFillQuantity()/BoardLotSizeBse);       
        DEBUG_VARSHOW(reqQryDebugLog(), " Second leg sell order placed at : ",TrdPrice + _BuyDiff);
        OrderIdOrderWrapperSellMap.insert(std::pair<API2::COMMON::OrderId*,API2_NEW::COMMON::OrderWrapper*>(_orderWrapperSecondSell->_orderId,_orderWrapperSecondSell));
        FirstLegBuyTradePrice = TrdPrice;
        FirstLegBuyTradedQty  +=  confirmation.getLastFillQuantity();
    }

    if(confirmation.getOrderMode() == API2::CONSTANTS::CMD_OrderMode_SELL)
    {
        //DEBUG_MESSAGE(reqQryDebugLog(), "--------------- Trade Event Starts for 1st Leg Sell ----------------");
        API2_NEW::COMMON::OrderWrapper *_orderWrapperSecondBuy = new API2_NEW::COMMON::OrderWrapper(_FeedInstrumentNse,_BuySide,this,AccountDetailNse);
        _orderWrapperSecondBuy->reset();
        OrderIdOrderWrapperBuyMap[_orderWrapperSecondBuy->_orderId]= _orderWrapperSecondBuy;
        _orderWrapperSecondBuy->newOrder(risk,TrdPrice - _SellDiff ,TrdQty);
        DEBUG_VARSHOW(reqQryDebugLog(), " First leg sell traded at : ",TrdPrice);
        DEBUG_VARSHOW(reqQryDebugLog(), " First leg sell traded qty : ",confirmation.getLastFillQuantity()/BoardLotSizeBse);       
        DEBUG_VARSHOW(reqQryDebugLog(), " Second leg buy order placed at : ",TrdPrice - _SellDiff);
        OrderIdOrderWrapperBuyMap.insert(std::pair<API2::COMMON::OrderId*,API2_NEW::COMMON::OrderWrapper*>(_orderWrapperSecondBuy->_orderId,_orderWrapperSecondBuy));
        FirstLegSellTradePrice = TrdPrice;   
        FirstLegSellTradedQty +=  confirmation.getLastFillQuantity();  
    }
    }

    if(API2::DATA_TYPES::SYMBOL_ID(_FeedInstrumentNse->getSymbolId()) == confirmation.getSymbolId())
    {
         TrdPrice = confirmation.getLastFillPrice();
         TrdQty = confirmation.getLastFillQuantity();

    if(confirmation.getOrderMode() == API2::CONSTANTS::CMD_OrderMode_BUY)
    {
        //DEBUG_MESSAGE(reqQryDebugLog(), "--------------- Trade Event Starts for 2nd Leg Nse Buy ----------------");
        First_Sell_Traded_Price = FirstLegSellTradePrice;
        Second_Buy_Traded_Price = TrdPrice;
        dTradeDiff = First_Sell_Traded_Price - Second_Buy_Traded_Price;

                if(dTradeDiff < (_SellDiff - _Tolerance))
                {
                 // DEBUG_VARSHOW(reqQryDebugLog(), "_IsBidding  : ",_IsBidding);
                _IsBidding = "No";
                  Update_Text(1);
                 // DEBUG_VARSHOW(reqQryDebugLog(), "_IsBidding  : ",_IsBidding);
                }       

    // DEBUG_MESSAGE(reqQryDebugLog(), "Second leg buy wrapper reset : ");
     // DEBUG_FLUSH(reqQryDebugLog());
    }

    if(confirmation.getOrderMode() == API2::CONSTANTS::CMD_OrderMode_SELL)
    {
        //DEBUG_MESSAGE(reqQryDebugLog(), "--------------- Trade Event Starts for 2nd Leg Nse Sell ----------------");

        First_Buy_Traded_Price = FirstLegBuyTradePrice;
        Second_Sell_Traded_Price = TrdPrice;
        dTradeDiff = Second_Sell_Traded_Price - First_Buy_Traded_Price;

                if(dTradeDiff < (_BuyDiff - _Tolerance))
                {
                 // DEBUG_VARSHOW(reqQryDebugLog(), "_IsBidding  : ",_IsBidding);
                _IsBidding = "No";
                  Update_Text(1);
                 // DEBUG_VARSHOW(reqQryDebugLog(), "_IsBidding  : ",_IsBidding);
                }      

        // DEBUG_MESSAGE(reqQryDebugLog(), "Second leg Sell wrapper reset : ");
        // DEBUG_FLUSH(reqQryDebugLog());
    }
    }
    // DEBUG_MESSAGE(reqQryDebugLog(), "--------------- onPartialFill Ends ----------------");
}


/*
void CurrencyBseNse::onProcessOrderConfirmation(API2::OrderConfirmation &confirmation)
{	

    DEBUG_MESSAGE(reqQryDebugLog(), "--------------- onProcessOrderConfirmation Starts ----------------");
    DEBUG_FLUSH(reqQryDebugLog());
    DEBUG_MESSAGE(reqQryDebugLog(),"............................................");
    DEBUG_VARSHOW(reqQryDebugLog(), "getTraderId();: ",confirmation.getTraderId());
    DEBUG_VARSHOW(reqQryDebugLog(), "getStrategyId(): ",confirmation.getStrategyId());
    DEBUG_VARSHOW(reqQryDebugLog(), "getSymbolId(): ",confirmation.getSymbolId());
    DEBUG_VARSHOW(reqQryDebugLog(), "getLimitPrice(): ",confirmation.getLimitPrice());
    DEBUG_VARSHOW(reqQryDebugLog(), "getOrderMode(): ",confirmation.getOrderMode());
    DEBUG_VARSHOW(reqQryDebugLog(), "getOrderPrice(): ",confirmation.getOrderPrice());
    DEBUG_VARSHOW(reqQryDebugLog(), "getOrderQuantity(): ",confirmation.getOrderQuantity());
    DEBUG_VARSHOW(reqQryDebugLog(), "getPendingQuantity(): ",confirmation.getOrderQuantity() - confirmation.getLastFillQuantity());
    DEBUG_VARSHOW(reqQryDebugLog(), "getLastFillPrice(): ",confirmation.getLastFillPrice());
    DEBUG_VARSHOW(reqQryDebugLog(), "getIOCCanceledQuantity(): ",confirmation.getIOCCanceledQuantity());
    DEBUG_VARSHOW(reqQryDebugLog(), "getLastFillQuantity(): ",confirmation.getLastFillQuantity());
    DEBUG_VARSHOW(reqQryDebugLog(), "getTradeId(): ",confirmation.getTradeId());
    DEBUG_VARSHOW(reqQryDebugLog(), "getOrderStatus(): ",confirmation.getOrderStatus());
    DEBUG_VARSHOW(reqQryDebugLog(), "getClOrderId(): ",confirmation.getClOrderId());
    DEBUG_VARSHOW(reqQryDebugLog(), "getExchangeOrderId(): ",confirmation.getExchangeOrderId());
    DEBUG_VARSHOW(reqQryDebugLog(), "getExchangeEntryTime(): ",confirmation.getExchangeEntryTime());
    DEBUG_VARSHOW(reqQryDebugLog(), "getExchangeModifyTime(): ",confirmation.getExchangeModifyTime());
    DEBUG_FLUSH(reqQryDebugLog());

    API2::DATA_TYPES::RiskStatus risk;
    if(API2::DATA_TYPES::SYMBOL_ID(_BseTradeInstr->getSymbolId()) == confirmation.getSymbolId()
            && confirmation.getOrderMode() == API2::CONSTANTS::CMD_OrderMode_BUY
            && confirmation.getOrderStatus() == 1)
    {
            API2::COMMON::MktData *mktDataNse = reqQryMarketData(_FeedInstrumentNse->getSymbolId());
            BuyPriceNse = mktDataNse->getBidPrice(0);
            if(_Depth == 0)
            BuyQuotePrice = BuyPriceNse - _BuyDiff;

            if(_Depth == 1)
            BuyQuotePrice = BuyPriceNse1 - _BuyDiff;

            OrderPrice = confirmation.getOrderPrice();
                if(OrderPrice > 0 && BuyQuotePrice > 0 && BuyPriceNse > 0 && OrderPrice > BuyQuotePrice)
                {
                    _orderWrapperBuy.cancelOrder(risk);
                     DEBUG_MESSAGE(reqQryDebugLog(), "--------------- Buy Cancelled order ----------------");
                     //DEBUG_FLUSH(reqQryDebugLog());
                }
    }


    if(API2::DATA_TYPES::SYMBOL_ID(_BseTradeInstr->getSymbolId()) == confirmation.getSymbolId()
            && confirmation.getOrderMode() == API2::CONSTANTS::CMD_OrderMode_SELL
            && confirmation.getOrderStatus() == 1)
    {
        API2::COMMON::MktData *mktDataNse = reqQryMarketData(_FeedInstrumentNse->getSymbolId());
        SellPriceNse = mktDataNse->getAskPrice(0);
        if(_Depth == 0)
        SellQuotePrice = SellPriceNse + _SellDiff;

        if(_Depth == 1)
        SellQuotePrice = SellPriceNse1 + _SellDiff;

        OrderPrice = confirmation.getOrderPrice();

         if(OrderPrice > 0 && SellQuotePrice > 0 && SellPriceNse > 0 && OrderPrice < SellQuotePrice)
             {
                _orderWrapperSell.cancelOrder(risk);
                 DEBUG_MESSAGE(reqQryDebugLog(), "--------------- Sell Cancelled order ----------------");
                 //DEBUG_FLUSH(reqQryDebugLog());
             }
    }

    // DEBUG_MESSAGE(reqQryDebugLog(), "--------------- onProcessOrderConfirmation Ends ----------------");
    // DEBUG_FLUSH(reqQryDebugLog());	
}
*/

void CurrencyBseNse::onCancelRejected(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId)
{

    //DEBUG_MESSAGE(reqQryDebugLog(), "onCancelRejected");

    //DEBUG_FLUSH(reqQryDebugLog());

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
     // DEBUG_MESSAGE(reqQryDebugLog(), "------------ onCancelRejected ends");
}

void CurrencyBseNse::onReplaceRejected(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId)
{
    //DEBUG_MESSAGE(reqQryDebugLog(), "onReplaceRejected");
    //DEBUG_FLUSH(reqQryDebugLog());

	API2::DATA_TYPES::RiskStatus risk;
    if( _orderWrapperBuy._orderId == orderId)
    {
        _orderWrapperBuy._isPendingCancel = false;
        _orderWrapperBuy._isPendingNew = false;
        _orderWrapperBuy._isPendingReplace = false;
        _orderWrapperBuy.reset();
        _orderWrapperBuy.cancelOrder(risk);
    }

    if( _orderWrapperSell._orderId == orderId)
    {
       _orderWrapperSell._isPendingCancel = false;
       _orderWrapperSell._isPendingNew = false;
       _orderWrapperSell._isPendingReplace = false;
       _orderWrapperSell.reset();      
       _orderWrapperSell.cancelOrder(risk);
    }   

     //DEBUG_MESSAGE(reqQryDebugLog(), "onReplaceRejected ends");
}

/*void CurrencyBseNse::onTimerEvent()
{
    //DEBUG_MESSAGE(reqQryDebugLog(), "------------Inside onTimerEvent ");
   // DEBUG_FLUSH(reqQryDebugLog());
   // reqTimerEvent(5000000);
    //DEBUG_MESSAGE(reqQryDebugLog(), "------------Inside onTimerEvent out ");
}*/

void CurrencyBseNse::onNewReject(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId)
{
    // DEBUG_MESSAGE(reqQryDebugLog(), "------------Inside onNewReject");    
    // DEBUG_VARSHOW(reqQryDebugLog(), "confirmation.getErrorCode()  : ",confirmation.getErrorCode());
    // DEBUG_VARSHOW(reqQryDebugLog(), "confirmation.getOrderStatus(  : ",confirmation.getOrderStatus());   
    // DEBUG_MESSAGE(reqQryDebugLog(),"Fresh Order Rejection...Strategy Terminate......");
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
