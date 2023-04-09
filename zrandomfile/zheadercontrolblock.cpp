#include "zheadercontrolblock.h"

#include <zxml/zxmlprimitives.h>

ZHeaderControlBlock::ZHeaderControlBlock()
{
  clear();
}

void ZHeaderControlBlock::clear(void)
{
  memset(this,0,sizeof(ZHeaderControlBlock));
  //    StartSign=cst_ZSTART;
  //    BlockId = ZBID_FileHeader ;
  //    ZRFVersion=__ZRF_VERSION__;
  //    EndSign=cst_ZEND;
  OffsetReserved = sizeof(ZHeaderControlBlock_Export); // because Reserved block just starts after ZHeaderControlBlock
  return;
}

ZHeaderControlBlock& ZHeaderControlBlock::_copyFrom(const ZHeaderControlBlock& pIn)
{
  Lock=pIn.Lock;
  LockOwner=pIn.LockOwner;
  FileType=pIn.FileType;
  OffsetFCB=pIn.OffsetFCB;
  OffsetReserved=pIn.OffsetReserved;
  SizeReserved=pIn.SizeReserved;
  return *this;
}
ZHeaderControlBlock&
ZHeaderControlBlock::_fromHCBE(const ZHeaderControlBlock_Export* pIn)
{
  FileType = pIn->FileType; /* FileType does not need to be reversed */
  OffsetFCB = pIn->OffsetFCB;
  OffsetReserved = pIn->OffsetReserved;
  SizeReserved = pIn->SizeReserved;
  Lock = pIn->Lock;
  LockOwner = pIn->LockOwner;
  return *this;
}

size_t
ZHeaderControlBlock::_exportAppend(ZDataBuffer& pZDBExport)
{
  ZHeaderControlBlock_Export wHCBe;
  wHCBe.set(*this);
  wHCBe.serialize();
  pZDBExport.appendData(&wHCBe,sizeof(wHCBe));
  return sizeof(wHCBe);
}



ZHeaderControlBlock_Export&
ZHeaderControlBlock_Export::_copyFrom(ZHeaderControlBlock_Export& pIn)
{
  StartSign=pIn.StartSign;
  BlockId=pIn.BlockId;
  EndianCheck=pIn.EndianCheck;

  ZRFVersion=pIn.ZRFVersion;
  FileType = pIn.FileType; /* FileType does not need to be reversed */
  OffsetFCB = pIn.OffsetFCB;
  OffsetReserved = pIn.OffsetReserved;
  SizeReserved = pIn.SizeReserved;
  Lock = pIn.Lock;
  LockOwner = pIn.LockOwner;

  EndSign=pIn.EndSign;
  return *this;
}

void ZHeaderControlBlock_Export::set(const ZHeaderControlBlock& pIn)
{
  StartSign = cst_ZBLOCKSTART;
  EndSign = cst_ZBLOCKEND;
  EndianCheck = cst_EndianCheck_Normal;

  ZRFVersion = __ZRF_VERSION__;

  FileType = pIn.FileType; /* FileType does not need to be reversed */
  OffsetFCB = pIn.OffsetFCB;
  OffsetReserved = pIn.OffsetReserved;
  SizeReserved = pIn.SizeReserved;
  Lock = pIn.Lock;
  LockOwner = pIn.LockOwner;
  return ;
}

void ZHeaderControlBlock_Export::setFromPtr(const unsigned char* &pPtrIn)
{
  ZHeaderControlBlock_Export* wHCBe=(ZHeaderControlBlock_Export*)pPtrIn;

  StartSign= wHCBe->StartSign;
  EndSign= wHCBe->EndSign;
  EndianCheck= wHCBe->EndianCheck;
  ZRFVersion = wHCBe->ZRFVersion;

  FileType = wHCBe->FileType; /* FileType does not need to be reversed */
  OffsetFCB = wHCBe->OffsetFCB;
  OffsetReserved = wHCBe->OffsetReserved;
  SizeReserved = wHCBe->SizeReserved;
  Lock = wHCBe->Lock;
  LockOwner = wHCBe->LockOwner;
  pPtrIn += sizeof(ZHeaderControlBlock_Export);
  return ;
}

ZHeaderControlBlock&
ZHeaderControlBlock_Export::toHCB(ZHeaderControlBlock& pOut)
{
  if (isReversed())
    fprintf(stderr,"ZHeaderControlBlock_Export-W-NOTREV Header control block is updated with a reversed(serialized) values.\n");

  pOut.FileType = FileType; /* FileType does not need to be reversed */
  pOut.OffsetFCB = OffsetFCB;
  pOut.OffsetReserved = OffsetReserved;
  pOut.SizeReserved = SizeReserved;
  pOut.Lock = Lock;
  pOut.LockOwner = LockOwner;

  return pOut;
}


void
ZHeaderControlBlock_Export::serialize()
{
  if (!is_little_endian())
    return ;
  if (isReversed())
  {
    fprintf (stderr,"ZHeaderControlBlock_Export::serialize-W-ALRDY ZHeaderControlBlock already serialized. \n");
    return;
  }
  _convert();
}
void
ZHeaderControlBlock_Export::deserialize()
{
  if (!is_little_endian())
    return ;
  if (isNotReversed())
  {
    fprintf (stderr,"ZHeaderControlBlock_Export::deserialize-W-ALRDY ZHeaderControlBlock already deserialized. \n");
    return;
  }
  _convert();
}
void
ZHeaderControlBlock_Export::_convert()
{
  if (!is_little_endian())
    return ;
/* BlockId (byte), StartSign EndSign(palyndromas) do not need to be reversed */

  EndianCheck=reverseByteOrder_Conditional<uint16_t>(EndianCheck);
  ZRFVersion=reverseByteOrder_Conditional<unsigned long>(ZRFVersion);
//  FileType = FileType; /* FileType does not need to be reversed */
  OffsetFCB = reverseByteOrder_Conditional<zaddress_type>(OffsetFCB);
  OffsetReserved = reverseByteOrder_Conditional<zaddress_type>(OffsetReserved);
  SizeReserved = reverseByteOrder_Conditional<zsize_type>(SizeReserved);
  Lock = reverseByteOrder_Conditional<zlockmask_type>(Lock);
  LockOwner = reverseByteOrder_Conditional<pid_t>(LockOwner);
} //_convert


ZStatus
ZHeaderControlBlock::_import(const unsigned char *&pPtrIn)
{
  ZHeaderControlBlock_Export wHCBe;
  wHCBe.setFromPtr(pPtrIn);
  wHCBe.deserialize();

  if ((wHCBe.BlockId!=ZBID_HCB)||(wHCBe.StartSign!=cst_ZBLOCKSTART))
  {
    ZException.setMessage("ZHeaderControlBlock::_import",
        ZS_BADFILEHEADER,
        Severity_Severe,
        "invalid header block content found Start marker <%X> ZBlockId <%X>. One of these is invalid (or both are).",
        wHCBe.StartSign,
        wHCBe.BlockId);
    return  ZS_BADFILEHEADER;
  }
  if (wHCBe.ZRFVersion!=__ZRF_VERSION__)
  {
    ZException.setMessage("ZHeaderControlBlock::_import",
        ZS_BADFILEHEADER,
        Severity_Severe,
        "invalid header block version : found version <%ld> while current version is <%ld>.",
        wHCBe.ZRFVersion,
        __ZRF_VERSION__);
    return  ZS_BADFILEHEADER;
  }

  _fromHCBE(&wHCBe);

//  pPtrIn += sizeof(ZHeaderControlBlock_Export);
  return  ZS_SUCCESS;
}//_import

utf8String ZHeaderControlBlock::toXml(int pLevel)
{
  int wLevel=pLevel;
  utf8String wReturn;
  wReturn = fmtXMLnode("zheadercontrolblock",pLevel);
//  fmtXMLaddInlineComment(wReturn," all children fields of zheadercontrolblock are not modifiable ");
  wLevel++;

  /* NB: Lock and LockOwner are not exported to xml */

  wReturn+=fmtXMLuint("filetype",  FileType,wLevel);  /* uint8_t */
  wReturn+=fmtXMLulong("zrfversion",  __ZRF_VERSION__,wLevel);  /* unsigned long */

  wReturn+=fmtXMLuint64("offsetfcb",OffsetFCB,wLevel);
  wReturn+=fmtXMLuint64("offsetreserved",OffsetReserved,wLevel);

  wReturn+=fmtXMLuint64("sizereserved",SizeReserved,wLevel);  /* uint16 must be casted */

  wReturn += fmtXMLendnode("zheadercontrolblock",pLevel);
  return wReturn;
} // ZRandomFile::toXml

int ZHeaderControlBlock::fromXml(zxmlNode* pHCBRootNode, ZaiErrors* pErrorlog)
{
  zxmlElement *wRootNode;
  utfcodeString wXmlHexaId;
  utf8String wValue;
  utfcodeString wCValue;
  bool wBool;
  unsigned int wInt;
  ZStatus wSt = pHCBRootNode->getChildByName((zxmlNode *&) wRootNode, "zheadercontrolblock");
  if (wSt != ZS_SUCCESS) {
    pErrorlog->logZStatus(
        ZAIES_Error,
        wSt,
        "ZHeaderControlBlock::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status "
        "<%s>",
        "zheadercontrolblock",
        decode_ZStatus(wSt));
    return -1;
  }

  if (XMLgetChildInt64(wRootNode, "offsetfcb", OffsetFCB, pErrorlog)< 0) {
    fprintf(stderr,
        "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "offsetfcb");
    }
  if (XMLgetChildInt64(wRootNode, "offsetreserved", OffsetReserved, pErrorlog)< 0) {
      fprintf(stderr,
        "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "offsetreserved");
      }
    if (XMLgetChildUInt64(wRootNode, "sizereserved", SizeReserved, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "default.",
          "sizereserved");
      }

  if (XMLgetChildUInt(wRootNode, "filetype", wInt, pErrorlog)< 0) {
    fprintf(stderr,
        "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "filetype");
      }
      else
        FileType = (ZFile_type)wInt;


  return (int)pErrorlog->hasError();
}//ZHeaderControlBlock::fromXml

