#ifndef ZNATURALFROMURF_H
#define ZNATURALFROMURF_H
/**
  This header file holds the conversion routines from universal record format type to natural data type format
  */
#include <ztoolset/zerror.h>
#include <ztoolset/zlimit.h>
#include <ztoolset/zexceptionmin.h>
#include <zindexedfile/ztypetype.h>
#include <zindexedfile/zdatatype.h>
#include <ztoolset/zatomicconvert.h>
template <class _Tp>
ZStatus
setBlobfURF(_Tp* pNatural,
                ZDataBuffer& pURFInData,
                ZType_type &pSourceType,
                int32_t &pSourceArrayCount,
                ZType_type &pTargetType,
                int32_t &pTargetArrayCount);

ZStatus
setZDateNfURF (void* pNatural,
                unsigned char *pURFData,
                ZType_type &pSourceType,
                int32_t &pSourceArrayCount,
                ZType_type &pTargetType,
                int32_t &pTargetArrayCount);

ZStatus _get_121_BlobNfURF(void* pValue,unsigned char* pURFData);

ZStatus _get_121_ZDateFullNfURF(void* pValue, unsigned char* pURFData);
ZStatus get_121_ZDateNfURF(void* pValue, unsigned char *pURFData);


ZStatus get_121_CheckSumNfURF(void* pValue,unsigned char* pURFData);

ZStatus get_ZStringNfURF(void* pValue, unsigned char* pURFData);


ZStatus _get_121_charFixedStringNfURF(void* pValue, unsigned char* pURFData);
ZStatus _get_121_utf8FixedStringNfURF(void* pValue, unsigned char* pURFData);
ZStatus get_121_utf16FixedStringNfURF(void* pValue, unsigned char* pURFData);
ZStatus get_121_utf32FixedStringNfURF(void* pValue, unsigned char* pURFData);

ZStatus _get_121_charVaryingStringNfURF(void* pValue, unsigned char* pURFData);
ZStatus get_121_utf8VaryingStringNfURF(void* pValue, unsigned char* pURFData);
ZStatus get_121_utf16VaryingStringNfURF(void* pValue, unsigned char* pURFData);
ZStatus get_121_utf32VaryingStringNfURF(void* pValue, unsigned char* pURFData);


/**
 * @brief setFieldURFfChar converts/endian process pNatural value a ZType_Char | ZType_Pointer to any available data struture
 *          and returns its URF (universal record format) data
 *
 *          Using _exportURF facilities of each class used
 *          For Natural Arrays (char or wchar_t), URF header is generated.
 *
 * @param pNatural          pointer to data (char*) as a void* pointer
 * @param pURFData          out URF value as a ZDataBuffer
 * @param pSourceType       Mandatory
 * @param pSourceSize       Mandatory
 * @param pSourceArrayCount Mandatory
 * @param pTargetType       Mandatory
 * @param pTargetArrayCount Mandatory
 * @return
 */
ZStatus
setCharfURF(void* pNatural,
                ZDataBuffer *pURFData,
                ZType_type &pTargetType,
                uint32_t &pTargetArrayCount);


//==========================Main entry points / data type for setFieldURF_T====================================

unsigned char* _getURFHeaderData(unsigned char* pURF_Ptr,
                                 ZTypeBase &pZType,
                                 uint64_t &pUniversalSize,
                                 uint64_t &pNaturalSize,
                                 uint16_t &pCanonical,
                                 uint16_t &pEffectiveUSize,
                                 uint64_t &pHeaderSize,
                                 unsigned char *&pEffectiveDataPtr);

size_t  _getURFHeaderSize (ZTypeBase &pZType);


template <class _Tp>
/**
 * @brief setFieldNfURF_T set field natural value from urf towards atomic data type :
 *  it may dynamically cast value to another atomic data natural format.
 * Remark: 'atomic' means integral or floating point.
 *
 * @param pValue
 * @param pURFData
 * @param pTargetType
 * @param pTargetArrayCount
 * @param pTargetNSize
 * @param pTargetUSize
 * @return
 */
static inline
ZStatus  setFieldNfURF_T (typename std::enable_if_t<std::is_integral<_Tp>::value|std::is_floating_point<_Tp>::value,_Tp> &pValue,
                          unsigned char* pURFData,        // pointer to input data in URF format
                          ZTypeBase& pTargetType,      // target type (given by RDic)

                          uint64_t &pTargetNSize,       // source natural size(out)
                          uint64_t &pTargetUSize,       // source universal size (URF data size minus header size)(out)
                          uint16_t &pTargetArrayCount        // target array count
                          )
{
_MODULEINIT_

ZStatus wSt;
uint16_t wCanonical , wEffectiveUSize;
uint64_t wSourceUSize,wSourceNSize,wHeaderSize;
ZTypeBase wSourceType;

unsigned char* wData_Ptr= _getURFHeaderData(pURFData,
                                            wSourceType,
                                            wSourceUSize,
                                            wSourceNSize,
                                            wCanonical,
                                            wEffectiveUSize,
                                            wHeaderSize,
                                            wData_Ptr);
    // Get the target (natural) data characteristics : type, sizes, array count (NB: Source data infos is given by URF)

    wSt=_getZType_T<_Tp>(pValue,pTargetType,pTargetNSize,pTargetUSize,pTargetArrayCount); //
    if (wSt!=ZS_SUCCESS)
            return wSt;

    fprintf (stdout,
             "%s-Atomic>> getting field value from URF source type <%x> <%s> to Natural target type <%x><%s>\n",
            _GET_FUNCTION_NAME_,
            wSourceType,
            decode_ZType(wSourceType),
            pTargetType,
            decode_ZType(pTargetType));


    if (wSourceType!=pTargetType)
        {
        if (!(wSourceType&ZType_Atomic)) // target type must be atomic
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_INVTYPE,
                                  Severity_Severe,
                                  "Incompatible source and target data type : cannot convert from <%x> <%s> to <%x> <%s>.",
                                  wSourceType,
                                  decode_ZType(wSourceType),
                                  pTargetType,
                                  decode_ZType(pTargetType));
            _RETURN_ ZS_INVTYPE;
            }

        // if atomic but different type then cast it to target type

        wSt=_castAtomicToNfU_T<_Tp>(pValue,wData_Ptr,wSourceType);
        }
        else
        _getAtomicNfU_T_Ptr<_Tp>(pValue,wData_Ptr,wSourceType); // if not, simply depack
    if (wSt!=ZS_SUCCESS)
                _RETURN_ wSt;

    pTargetArrayCount=1;
    _RETURN_ ZS_SUCCESS;
} //setFieldURF_T for Atomic





/*
ZStatus
_getZDateFullNfURF(void* pValue, unsigned char* pURFData, ZTypeBase pTargetType, uint64_t pTargetNSize, uint32_t pTargetArrayCount=1);

ZStatus
_getZDateNfURF(void* pValue, unsigned char *pURFData, ZTypeBase pTargetType, uint64_t pTargetNSize, uint32_t pTargetArrayCount=1);
ZStatus
_getFixedWStringNfURF(void* pValue, unsigned char *pURFData, ZTypeBase pTargetType, uint64_t pSourceNSize);
ZStatus
_getFixedCStringNfURF(void* pValue, unsigned char *pURFData, ZTypeBase pTargetType, uint64_t pSourceNSize);
ZStatus
_getVaryingCStringNfURF(void* pValue, unsigned char *pURFData, ZTypeBase pTargetType, uint64_t pSourceSize);
ZStatus
_getVaryingWStringNfURF(void* pValue, unsigned char* pURFData, ZTypeBase pTargetType, uint64_t pSourceSize);
ZStatus
_getCheckSumNfURF(void* pValue,unsigned char* pURFData,ZTypeBase pTargetType);
ZStatus
_getBlobNfURF(void* pValue, unsigned char* pURFData, ZTypeBase pTargetType);
*/


template <class _Tp>
/**
 * @brief setFieldNfURF_T set field natural value from urf towards class objects : conversion is made strictly 1 to 1.
 *
 * An errored ZStatus is issued if data type does not correspond strictly to the defined one in URF data.
 *
 *  - ZDate
 *  - ZDateFull
 *  - Z
 *
 * @param pValue
 * @param pURFData
 * @param pTargetType       ZType_type of the target data to receive URF data. It musts correspond strictly to the one defined in URF data.
 * @param pTargetArrayCount target array count (out) also called canonical size: it corresponds to the number of character units.
 * @param pTargetNSize
 * @param pTargetUSize
 * @return
 */
static
ZStatus  setFieldNfURF_T (typename std::enable_if_t<std::is_class<_Tp>::value,_Tp> &pValue,
                          unsigned char* pURFData,     // pointer to input data in URF format
                          ZTypeBase& pTargetType,      // target type (given by RDic)

                          uint64_t &pTargetNSize,       // source natural size(out)
                          uint64_t &pTargetUSize,       // source universal size (URF size minus header size)(out)
                          uint16_t &pTargetArrayCount   // target array count (out) also called canonical size
                          )
{
_MODULEINIT_

ZStatus wSt;
uint16_t wSourceCanonical , wEffectiveUSize;
uint64_t wSourceUSize , wSourceNSize,wHeaderSize;
ZTypeBase wSourceType;
ZDataBuffer wUValue;

unsigned char* wData_Ptr= _getURFHeaderData(pURFData,
                                            wSourceType,
                                            wSourceUSize,
                                            wSourceNSize,
                                            wSourceCanonical,
                                            wEffectiveUSize,
                                            wHeaderSize,
                                            wData_Ptr);
    // Get the target (natural) data characteristics : type, sizes, array count (NB: Source data infos is given by URF)

    if (wSourceType!=pTargetType)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Severe,
                              "Invalid Class data type while getting field value from URF source type <%X> <%s> to Natural target type <%X><%s>\n",
                              wSourceType,
                              decode_ZType(wSourceType),
                              pTargetType,
                              decode_ZType(pTargetType));

        _RETURN_ ZS_INVTYPE;
        }

    wSt=_getZType_T<_Tp>(pValue,pTargetType,pTargetNSize,pTargetUSize,pTargetArrayCount); //
    if (wSt!=ZS_SUCCESS)
            return wSt;

    fprintf (stdout,
             "%s-Class>> assigning field value from URF source type <%X> <%s> to Natural target type <%X><%s>\n"
             "                  Source  Target\n"
             "   Universal size  %7ld  %ld\n"
             "   Natural   size  %7ld  %ld\n"
             "   Canonical count %7ld  %ld\n"
             "   URF Effective size %7ld\n"
             "   URF Header    size %7ld\n"
            ,
            _GET_FUNCTION_NAME_,
            wSourceType,
            decode_ZType(wSourceType),
            pTargetType,
            decode_ZType(pTargetType));
    switch (wSourceType)
    {
    case ZType_Blob:
    {
        return _get_121_BlobNfURF(&pValue,pURFData);
    }// ZType_Blob


   case ZType_ZDateFull: // convert from source ZDateFull to...
   {
       _RETURN_ _get_121_ZDateFullNfURF(&pValue,pURFData);
   }
   case ZType_ZDate: // convert from source ZDateFull to...
   {

       _RETURN_ get_121_ZDateNfURF(&pValue,pURFData);
   }
   case ZType_CheckSum: // convert from source ZDateFull to...
   {
       _RETURN_ get_121_CheckSumNfURF(&pValue,pURFData);
   }
   case ZType_Utf8FixedString:
   case ZType_Utf16FixedString:
   case ZType_Utf32FixedString:
   {
       _RETURN_ get_121_utfStringNfURF(&pValue,pURFData);
   }

   case ZType_Utf8VaryingString:
   case ZType_Utf16VaryingString:
   case ZType_Utf32VaryingString:
   {
       _RETURN_ get_121_utfStringNfURF(&pValue,pURFData);
   }

   default:
    {
    ZException.setMessage(_GET_FUNCTION_NAME_,
                          ZS_INVTYPE,
                          Severity_Severe,
                          "Invalid Class data type while getting field value from URF source type <%X> <%s> to Natural target type <%X><%s>\n",
                          wSourceType,
                          decode_ZType(wSourceType),
                          pTargetType,
                          decode_ZType(pTargetType));

    _RETURN_ ZS_INVTYPE;
    }
   }//switch
    _RETURN_ ZS_SUCCESS;
} //setFieldNfURF_T for Class


template <class _Tp>
static inline
ZStatus  setFieldNfURF_T (typename std::enable_if_t<std::is_array<_Tp>::value,_Tp> &pValue,
                          unsigned char* pURFData,        // pointer to input data in URF format
                          ZTypeBase& pTargetType,      // target type (given by RDic)

                          uint64_t &pTargetNSize,       // source natural size(out)
                          uint64_t &pTargetUSize,       // source universal size (URF size minus header size)(out)
                          uint16_t &pTargetArrayCount        // target array count (out)
                          )
{
ZStatus wSt;
uint16_t wSourceCanonical,wEffectiveUSize;
uint64_t wSourceUSize,wSourceNSize,wHeaderSize;
ZTypeBase wSourceType;
ZDataBuffer wUValue;

unsigned char* wData_Ptr= _getURFHeaderData(pURFData,
                                            wSourceType,
                                            wSourceUSize,
                                            wSourceNSize,
                                            wSourceCanonical,
                                            wEffectiveUSize,
                                            wHeaderSize,
                                            wData_Ptr);
    // Get the target (natural) data characteristics : type, sizes, array count (NB: Source data infos is given by URF)

    wSt=_getZType_T<_Tp>(pValue,pTargetType,pTargetNSize,pTargetUSize,pTargetArrayCount); //
    if (wSt!=ZS_SUCCESS)
            return wSt;
    printf ("%s-Array>> getting field value from URF source type <%x> <%s> to Natural target type <%x><%s>\n",
            _GET_FUNCTION_NAME_,
            wSourceType,
            decode_ZType(wSourceType),
            pTargetType,
            decode_ZType(pTargetType));
    if (wSourceType!=pTargetType)
        {
       ZException.setMessage(_GET_FUNCTION_NAME_,
                             ZS_INVTYPE,
                             Severity_Severe,
                             "Incompatible source and target data type : cannot convert from <%x> <%s> to <%x> <%s>.",
                             wSourceType,
                             decode_ZType(wSourceType),
                             pTargetType,
                             decode_ZType(pTargetType));
       return ZS_INVTYPE;
        }
    wEffectiveUSize=wSourceCanonical;
    if (pTargetArrayCount<wSourceCanonical)
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_FIELDCAPAOVFLW,
                              Severity_Error,
                              " Target field capacity overflow for array count <%s> requested to <%s>",
                              wSourceCanonical,
                              pTargetArrayCount);
        wSt=ZS_FIELDCAPAOVFLW;
        wEffectiveUSize=pTargetArrayCount;
    }
    ZTypeBase wTypeAtomic = wSourceType&ZType_AtomicMask;
    switch (wTypeAtomic)
    {
    case ZType_Char:
    case ZType_UChar:
    case ZType_U8:
        {

        uint8_t* wTarget_Ptr=static_cast<uint8_t*>(pValue);
        memmove (wTarget_Ptr,wData_Ptr,wEffectiveUSize);
        return wSt;
        }
    case ZType_S8:
        {
        int8_t* wTarget_Ptr=static_cast<int8_t*>(pValue);
        int8_t* wSource_Ptr=static_cast<int8_t*>(wData_Ptr);
        for (size_t wi=0;wi< wEffectiveUSize;wi++)
                        wTarget_Ptr[wi]=_getAtomicNfU_T_Ptr<int8_t>(wTarget_Ptr[wi],(unsigned char*)&wSource_Ptr[wi],ZType_S8);
        return wSt;
        }
    case ZType_U16:
        {
        uint16_t* wTarget_Ptr=static_cast<uint16_t*>(pValue);
        uint16_t* wSource_Ptr=static_cast<uint16_t*>(wData_Ptr);
        for (size_t wi=0;wi< wEffectiveUSize;wi++)
                        wTarget_Ptr[wi]=_getAtomicNfU_T_Ptr<uint16_t>(wTarget_Ptr[wi],(unsigned char*)&wSource_Ptr[wi],ZType_U16);
        return wSt;
        }
    case ZType_S16:
        {
        int16_t* wTarget_Ptr=static_cast<int16_t*>(pValue);
        int16_t* wSource_Ptr=static_cast<int16_t*>(wData_Ptr);
        for (size_t wi=0;wi< wEffectiveUSize;wi++)
                        wTarget_Ptr[wi]=_getAtomicNfU_T_Ptr<int16_t>(wTarget_Ptr[wi],(unsigned char*)&wSource_Ptr[wi],ZType_S16);
        return wSt;
        }
    case ZType_U32:
        {
        uint32_t* wTarget_Ptr=static_cast<uint32_t*>(pValue);
        uint32_t* wSource_Ptr=static_cast<uint32_t*>(wData_Ptr);
        for (size_t wi=0;wi< wEffectiveUSize;wi++)
                        wTarget_Ptr[wi]=_getAtomicNfU_T_Ptr<uint32_t>(wTarget_Ptr[wi],(unsigned char*)&wSource_Ptr[wi],ZType_U32);
        return wSt;
        }
    case ZType_S32:
        {
        int32_t* wTarget_Ptr=static_cast<int32_t*>(pValue);
        int32_t* wSource_Ptr=static_cast<int32_t*>(wData_Ptr);
        for (size_t wi=0;wi< wEffectiveUSize;wi++)
                        wTarget_Ptr[wi]=_getAtomicNfU_T_Ptr<int32_t>(wTarget_Ptr[wi],(unsigned char*)&wSource_Ptr[wi],ZType_S32);
        return wSt;
        }
    case ZType_U64:
        {
        uint64_t* wTarget_Ptr=static_cast<uint64_t*>(pValue);
        uint64_t* wSource_Ptr=static_cast<uint64_t*>(wData_Ptr);
        for (size_t wi=0;wi< wEffectiveUSize;wi++)
                        wTarget_Ptr[wi]=_getAtomicNfU_T_Ptr<uint64_t>(wTarget_Ptr[wi],(unsigned char*)&wSource_Ptr[wi],ZType_U64);
        return wSt;
        }
    case ZType_S64:
        {
        int64_t* wTarget_Ptr=static_cast<int64_t*>(pValue);
        int64_t* wSource_Ptr=static_cast<int64_t*>(wData_Ptr);
        for (size_t wi=0;wi< wEffectiveUSize;wi++)
                        wTarget_Ptr[wi]=_getAtomicNfU_T_Ptr<int64_t>(wTarget_Ptr[wi],(unsigned char*)&wSource_Ptr[wi],ZType_S64);
        return wSt;
        }
    case ZType_Float:
        {
        float* wTarget_Ptr=static_cast<float*>(pValue);
        float* wSource_Ptr=static_cast<float*>(wData_Ptr);
        for (size_t wi=0;wi< wEffectiveUSize;wi++)
                        wTarget_Ptr[wi]=_getAtomicNfU_T_Ptr<float>(wTarget_Ptr[wi],(unsigned char*)&wSource_Ptr[wi],ZType_Float);
        return wSt;
        }
    case ZType_Double:
        {
        double* wTarget_Ptr=static_cast<double*>(pValue);
        double* wSource_Ptr=static_cast<double*>(wData_Ptr);
        for (size_t wi=0;wi< wEffectiveUSize;wi++)
                        wTarget_Ptr[wi]=_getAtomicNfU_T_Ptr<double>(wTarget_Ptr[wi],(unsigned char*)&wSource_Ptr[wi],ZType_Double);
        return wSt;
        }
    case ZType_LDouble:
        {
        long double* wTarget_Ptr=static_cast<long double*>(pValue);
        long double* wSource_Ptr=static_cast<long double*>(wData_Ptr);
        for (size_t wi=0;wi< wEffectiveUSize;wi++)
                        wTarget_Ptr[wi]=_getAtomicNfU_T_Ptr<long double>(wTarget_Ptr[wi],(unsigned char*)&wSource_Ptr[wi],ZType_LDouble);
        return wSt;
        }
    default:
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Severe,
                              "Invalid atomic type <%lX> <%s> while converting Array ",
                              pTargetType,
                              decode_ZType(pTargetType));
        return ZS_INVTYPE;
    }
    }//witch (wTypeAtomic)

    return ZS_SUCCESS;
} //setFieldNfURF_T for Arrays

#ifdef __DEPRECATED_FIELD_CONVERSION__
template <class _Tp>
/**
 * @brief setFieldNfURF_T set field natural value from urf towards class objects
 *
 *  - ZDate
 *  - ZDateFull
 *  - Z
 *
 * @param pValue
 * @param pURFData
 * @param pTargetType
 * @param pTargetArrayCount
 * @param pTargetNSize
 * @param pTargetUSize
 * @return
 */
static inline
ZStatus  setFieldNfURF_T (typename std::enable_if_t<std::is_class<_Tp>::value,_Tp> &pValue,
                          unsigned char* pURFData,     // pointer to input data in URF format
                          ZTypeBase& pTargetType,      // target type (given by RDic)

                          uint64_t &pTargetNSize,       // source natural size(out)
                          uint64_t &pTargetUSize,       // source universal size (URF size minus header size)(out)
                          uint16_t &pTargetArrayCount   // target array count (out)
                          )
{
ZStatus wSt;
uint16_t wSourceCanonical , wEffectiveUSize;
uint64_t wSourceUSize , wSourceNSize,wHeaderSize;
ZTypeBase wSourceType;
ZDataBuffer wUValue;

unsigned char* wData_Ptr= _getURFHeaderData(pURFData,
                                            wSourceType,
                                            wSourceUSize,
                                            wSourceNSize,
                                            wSourceCanonical,
                                            wEffectiveUSize,
                                            wHeaderSize,
                                            wData_Ptr);
    // Get the target (natural) data characteristics : type, sizes, array count (NB: Source data infos is given by URF)

    wSt=_getZType_T<_Tp>(pValue,pTargetType,pTargetNSize,pTargetUSize,pTargetArrayCount); //
    if (wSt!=ZS_SUCCESS)
            return wSt;
    printf ("%s-Class>> getting field value from URF source type <%X> <%s> to Natural target type <%X><%s>\n",
            _GET_FUNCTION_NAME_,
            wSourceType,
            decode_ZType(wSourceType),
            pTargetType,
            decode_ZType(pTargetType));

    if (wSourceType==ZType_Blob)
    {
        if (pTargetType!=ZType_Blob)
        {
       ZException.setMessage(_GET_FUNCTION_NAME_,
                             ZS_INVTYPE,
                             Severity_Severe,
                             "Incompatible source and target data type : cannot convert from <%s> to <%s>.",
                             decode_ZType(wSourceType),
                             decode_ZType(pTargetType));
       return ZS_INVTYPE;
        }

        return _getBlobNfURF(&pValue,pURFData,pTargetType);
    }// ZType_Blob

   switch (wSourceType)
   {
   case ZType_ZDateFull: // convert from source ZDateFull to...
   {

       return _getZDateFullNfURF(&pValue,pURFData,pTargetType,pTargetNSize,pTargetArrayCount);
   }
   case ZType_ZDate: // convert from source ZDateFull to...
   {

       return _getZDateNfURF(&pValue,pURFData,pTargetType,pTargetNSize,pTargetArrayCount);
   }
   case ZType_CheckSum: // convert from source ZDateFull to...
   {

       return _getCheckSumNfURF(&pValue,pURFData,pTargetType);
   }
   case ZType_FixedCString:
   {
       return _getFixedCStringNfURF(&pValue,pURFData,pTargetType,wSourceNSize);
   }
   case ZType_VaryingCString:
   {
       return _getVaryingCStringNfURF(&pValue,pURFData,pTargetType,wSourceNSize);
   }
   case ZType_FixedWString:
   {
       return _getFixedWStringNfURF(&pValue,pURFData,pTargetType,wSourceNSize);
   }
   case ZType_VaryingWString:
   {
       return _getVaryingWStringNfURF(&pValue,pURFData,pTargetType,wSourceNSize);
   }
   }//switch
    return ZS_SUCCESS;
} //setFieldNfURF_T for Class


#endif //__DEPRECATED_FIELD_CONVERSION__


#endif // ZNATURALFROMURF_H
