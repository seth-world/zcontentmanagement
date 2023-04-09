/*
 * Gérard POLO 09/11/2015
 *
*
 *
 *
 */



#define __MAIN__


#include <QCoreApplication>

#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <stddef.h>

#include <zconfig.h>

#include <ztoolset/zarray.h>

#include <ztoolset/zerror.h>

#include <zio/zdir.h>

#include <zindexedfile/zmasterfile.h>

#include <typeinfo>
//#include <type_traits>

#include <arpa/inet.h>

/** T1 : simple cool example  : T1 is the class definition for ZSA.  T1 is also used as 'bulk' structure because its key field is named _Key */

struct Keystruct {
    char KeyContent[50];
};

struct T1 {
           char String1 [50]  ;
           float AtomicField  ;
};

struct T3 {
           char  String1 [50];
           long AtomicField [3]  ;
};
struct T2 {
    char String1 [50];
    long AtomicField;

};

CUF_struct<T1> myCUF;   //! for CommitUserFunction

 ZCommitStatus CUFunction(const T1&ZAMRow,ZOp pOperation,void *pData)
 {
ZCommitStatus wZCS;
    char *wData =(char *)pData;

    if (pOperation==ZO_Insert)
                printf ("Insert found\n");
    wZCS=ZCS_Success;

     return(wZCS);
 }


 void
 printIndexKey_0(ZMasterFile &ZPicture)

{
ZStatus wSt;

struct IndexKey_0_struct
{
    long AtomicField [3]  ;
    char String1 [50] ;
} IndexKey_0;


 fprintf (stdout,
          "\n ---------Index 0 Key dump----------\n"
//          "%5s <%15s> <%10s> <%50s>\n",
          "%5s <%10s> <%50s>\n",
          "Rank",
//          "Address",
          "AtomicField",
          "String1"
          );



 ZDataBuffer wIndexContent;
 long wi = 0;
 //wSt=ZPicture.getRawIndex(wRawIndexContent,wi,0L);


 wSt=ZPicture.getKeyIndexFields(wIndexContent,0L,0L);
 if (wSt!=ZS_SUCCESS)
            ZRFException.exit_abort();
 while (wSt==ZS_SUCCESS)
 {
     wIndexContent.moveOut<decltype(T3::AtomicField)>(IndexKey_0.AtomicField,0L);
     wIndexContent.moveOut<decltype(T3::String1)>(IndexKey_0.String1,sizeof(T3::AtomicField));
//     wIndexContent.moveOut<IndexKey_0_struct> (IndexKey_0); //! use ZDataBuffer template move to setup user defined struct ZIndexKey_0
//        wRawIndexContent.KeyContent.Dump(16,stdout);
     fprintf (stdout, "%5ld <%10ld> <%10ld> <%10ld> <%-50s>\n",
//     fprintf (stdout, "%5ld <%10f> <%-50s>\n",
              wi,
//              wIndexContent.ZMFaddress,
              IndexKey_0.AtomicField[0],
             IndexKey_0.AtomicField[1],
             IndexKey_0.AtomicField[2],
              IndexKey_0.String1
              );
     wi++;
     wSt=ZPicture.getKeyIndexFields(wIndexContent,wi,0L);
 }
 fprintf (stdout,"\n ---------End Index 0 Key dump----------\n");
 if (wSt!=ZS_OUTBOUNDHIGH)
         {
             ZRFException.exit_abort();
         }
 return;
 }//printIndexKey_0

 void
 printIndexKey_1(ZMasterFile &ZPicture)

{
ZStatus wSt;
 fprintf (stdout,
          "\n ---------Index 1 Key dump----------\n"
          "%5s <%15s> <%50s>\n",
          "Rank",
          "Address",
          "String1"
          );

 struct IndexKey_1_struct
 {
     char String1[50]  ;
 } IndexKey_1;

 ZIndexItem wRawIndexContent;
 ZPicture.zopen(ZRF_Read);
 long wi = 0;
 wSt=ZPicture.getRawIndex(wRawIndexContent,wi,1L);

 while (wSt==ZS_SUCCESS)
 {
     wRawIndexContent.KeyContent.moveOut<IndexKey_1_struct> (IndexKey_1); //! use ZDataBuffer template move to setup user defined struct ZIndexKey_0
//        wRawIndexContent.KeyContent.Dump(16,stdout);
     fprintf (stdout, "\n%5ld <%15lld> <%-50s>\n",
              wi,
              wRawIndexContent.ZMFaddress,
              IndexKey_1.String1
              );
     wi++;
     wSt=ZPicture.getRawIndex(wRawIndexContent,wi,1L);
 }
 fprintf (stdout,"\n ---------End Index 1 Key dump----------\n");
 if (wSt!=ZS_OUTBOUNDHIGH)
         {
             ZRFException.exit_abort();
         }
 return;
 }//printIndexKey_1

int main(int argc, char *argv[])
{
_MODULEINIT_ ;


    const size_t _Max_T1Cst = 11;
    const size_t _Max_T2Cst = 12;
    size_t wi;

//    zbs::ZResult wRes;
    ZStatus wSt ;
    char wBuf[50];


    struct T1 T1Cst[] =
    {
    {"my heart beats"        ,  -155 },     // row with multiple correspondance within Slave
    {"my heart beats for you",  -1024},      // no correspondance within slave
    {"and so beats my skin",    -152} ,
    {"xxxx",                    110},         // duplicate this row on ZIX1 key string
    {"this is one",             1 },
    {"aaaa   ",                 34 },
    {"eer",                     5},
    {"   dddd",                 250},
    {"jkfdlsakfj",              2025},      // row with no correspondance within Slave
    {"egadfk",                 25},
    {"aaaa   ",                12045 }
    };


    struct T2 T2Cst[] =
    {
    {"my heart beats"        ,  15500 },       // row with multiple correspondance within Slave
    {"my heart beats",         102400},
    {"my heart beats",          102401},
    {"and so beats my skin",    15200} ,
    {"xxxx",                    11000},
    {"this is one",             100 },
    {"aaaa   ",                 3400 },
    {"eer",                     500},
    {"   dddd",                 25000},
    {"jkfd",                     202500},       // row with no correspondance within Master
     {"egadfk",                 2500},
     {"aaaa   ",                1204500 }       // row with multiple correspondance within Master and slave (n , n) relationship
    };

    struct T3 T3Cst[] =
    {
    {"my heart beats"        ,  {110 ,-155 ,1} },     // row with multiple correspondance within Slave
    {"my heart beats for you",  {110 ,-1024,1} },      // no correspondance within slave
    {"and so beats my skin",    {110 ,-152, 1} } ,
    {"xxxx",                     {110, 110 ,1} },         // duplicate this row on ZIX1 key string
    {"this is one",             {1 , 110 ,1 }},
    {"aaaa   ",                 {34 , 110 ,1} },
    {"eer",                     {5, 110 ,1 }},
    {"   dddd",                 {250, 110 ,1 }},
    {"jkfdlsakfj",              {2025, 110 ,1 }},      // row with no correspondance within Slave
    {"egadfk",                 {25, 110 ,1 }},
    {"aaaa   ",                {12045, 110 ,1 } }
    };

    // +++++++++++++++Define the _Bulk structure for K1++++++++++++++++++++++
    #ifdef __GNUC__
            T1 T1_Obj ;
            constexpr const  size_t K1_Keyoffset = offsetof(T1,String1);
            constexpr const  size_t K1_remaining = sizeof(T1) - sizeof(T1_Obj.String1) - K1_Keyoffset ;

       #else
        #ifdef __clang__
            #pragma message("WARNING: clang is used and not GNU gcc. You have to define your own _Bulk structures - use gcc instead")
            #pragma message(__clang)
        #else
            #pragma message("WARNING: An unknown c++ compiler is used and not GNU gcc. You have to define your own _Bulk structures - use gcc instead")
        #endif
        constexpr size_t K1_Keyoffset=0;
        constexpr size_t K1_remaining=8;


    #endif



    struct K1_Bulk
    {
        char K1_Filler1[K1_Keyoffset];
        Keystruct _Key;
        char K1_Filler2[K1_remaining];
    };



    // +++++++++++++++Define the _Bulk structure for T2++++++++++++++++++++++
    #ifdef __GNUC__
            T2 T2_Obj ;
            constexpr const  size_t K2_Keyoffset = offsetof(T2,String1);
            constexpr const  size_t K2_remaining = sizeof(T2) - sizeof(T2_Obj.String1) - K2_Keyoffset ;

       #else
        #ifdef __clang__
            #pragma message("WARNING: clang is used and not GNU gcc. You have to define your own _Bulk structures - use gcc instead")
        #else
            #pragma message("WARNING: An unknown c++ compiler is used and not GNU gcc. You have to define your own _Bulk structures - use gcc instead")
        #endif
        constexpr size_t K2_Keyoffset=0;
        constexpr size_t K2_remaining=8;

    #endif
    struct K2_Bulk
    {
        char K2_Filler1[K2_Keyoffset];
        Keystruct _Key;
        char K2_Filler2[K2_remaining];
    };


//!---------- Varying blocks : include documents ------------------
//!
//! will be a T1 Structure plus a picture as ZDataBuffer
//!

//!---------------------------------------------------------------------------------
//!  Creating an Indexed ZMasterFile :
//!
//!     First instantiate a ZMasterFile with or whitout a file name
//!   (file name and file path can be mentionned later on using setPath method)
//!
//!     Second define key :
//!         for each desired key define key fields :
//!             for each field the ZIndexField_struct to specify each field composing the Key
//!         then store it in an ZArray<ZIndexField_struct> in the order of the key (not necessarily growing offset order)
//!
//!     Third create the Index file from ZMasterFile using : zcreateIndexFile method
//!
//!
//!
//ZStatus wSt;


//    if (zgetZType<decltype (T1::AtomicField)>(wType_mask,wLength,true)!=ZS_SUCCESS)
//                                    ZRFException.exit_abort();

    ZRandomFile ZPicture ;

    wSt=ZPicture.zcreate("/home/gerard/testindexedpictures.zrf",100000 );
    if (wSt!=ZS_SUCCESS)
                ZRFException.exit_abort();
    wSt=ZPicture.zopen("/home/gerard/testindexedpictures.zrf",ZRF_Read);
    if (wSt!=ZS_SUCCESS)
                ZRFException.exit_abort();
/*
    if (ZPicture.zclearFile()!=ZS_SUCCESS)// test clear file
                ZRFException.exit_abort();
   ZRandomFile::zsurfaceScan("/home/gerard/testindexedpictures.zmf");
*/
ZDataBuffer wRecord;
ZIndexControlBlock wICB;

//! define index keys
//!
//!             one index key on AtomicField then String1
//!
  typedef decltype (T3::AtomicField) T3Atomic;
  wSt=wICB.zaddFieldToZKDic<T3Atomic>("AtomicField",offsetof (T3,AtomicField));
  typedef decltype (T3::String1) T3String1;
  wSt=wICB.zaddFieldToZKDic<T3String1> ("String1",offsetof (T3,String1));

    //! dumping field list content
    //!
    //!
    wICB.ZKDic.print(stdout);


//! Key extraction tests  (negative Atomic values)
//!
/*
ZDataBuffer wKeyT;
    for (long wi=0;wi<11; wi++ )
            {
            wRecord.setData( &T1Cst[wi],sizeof(T1));
            if (wICB.zkeyValueExtraction(wRecord,wKeyT)!=ZS_SUCCESS)
                            ZRFException.exit_abort();
            fprintf (stdout,"Key for rank %d Atomic <%ld> String1 <%s>\n",
                     wi,
                     T1Cst[wi].AtomicField,
                     T1Cst[wi].String1);
            wKeyT.Dump();
            }

*/
#ifdef __COMMENT__
//! Fields list is created then create the Index key itself (create the physical files)
//!     At this occasion, you should mention the key specifications
//!

    wSt=ZPicture.zcreateIndexFile(wICB.ZKDic,           //! the list that just has been set
                                  "First Index",        //! user defined name (not used internally)
                                  ZIF_Binary,            //! type of key (for compare processing)
                                  true,                 //! Autorebuild is set to true
                                  ZST_NODUPLICATES);    //! Allowing duplicates or not

    if (wSt!=ZS_SUCCESS)
                ZRFException.exit_abort();


//!
//! At this stage
//!         Master file has been created with its header
//!         Index file has been created with its own header
//!
//!     Nota Bene : if you wish to store Index files (All Index files) you need to specify a location
//!                 using the method setIndexFilesDirectoryPath giving a valid, existing directory specification
//!                 This action should be taken BEFORE calling zcreateIndexFile method.
//!

#endif // __COMMENT__

    if ((wSt=ZPicture.zopen(ZRF_Write))!=ZS_SUCCESS) //! load document
                {
                    ZRFException.exit_abort();
                }
/*    if (ZPicture.setGrabFreeSpace(true)!=ZS_SUCCESS)
                                   ZRFException.exit_abort();*/
/*    if (ZPicture.setHighwaterMarking(true)!=ZS_SUCCESS)
                                   ZRFException.exit_abort();
*/
   const char * PictureFiles [] = {
        "/home/gerard/Pictures/DSC_01231.jpg",
        "/home/gerard/Pictures/DSC_01232.jpg",
        "/home/gerard/Pictures/DSC_0123.JPG",           // to be deleted idx 2
        "/home/gerard/Pictures/DSC_0146.JPG",
        "/home/gerard/Pictures/DSC_0394.JPG",           // to be deleted idx 4
        "/home/gerard/Pictures/DSC_0414.JPG",
        "/home/gerard/Pictures/Gelato.mp4",
        "/home/gerard/Pictures/mymatelas-facture2016-04-26.pdf",
        "/home/gerard/Pictures/QItemDelegate Class | Qt Widgets 5.7.html", // here idx 8
        "/home/gerard/Documents/DSC_0265.JPG",          //to be added idx 9
        "/home/gerard/Documents/DSC_0267.JPG",          // to be added idx 10
        "/home/gerard/Documents/DSCN0971.JPG"           // to be added idx 11
    };



    ZTimer wTimer;
    wTimer.start();

    uriString wURIPicture;
    ZDataBuffer wPicture;
   for (int wi = 0;wi < 9;wi++)
        {
 //           wRecord.setData( &T1Cst[wi],sizeof(T1));  //! get the occurrence of T1
            wRecord.setData( &T3Cst[wi],sizeof(T3));  //! get the occurrence of T1
            wURIPicture = PictureFiles[wi];         //! set document path
            if ((wSt=wURIPicture.loadContent(wPicture))!=ZS_SUCCESS) //! load document
                        {
                            ZException.exit_abort();
                        }
            wRecord += wPicture;                    //! append document content to record buffer
            if ((wSt=ZPicture.zadd(wRecord))!=ZS_SUCCESS) //! load document
                        {
                            ZRFException.exit_abort();
                        }
//             printIndexKey_0(ZPicture);
        }






   if ((wSt=ZPicture.zremove(3)!=ZS_SUCCESS) //! remove 3rd rank
               {
                   ZRFException.exit_abort();
               }
   ZRandomFile::zsurfaceScan("/home/gerard/testindexedpictures.zrf");

    ZPicture.zreorgFile();

     ZPicture.zclose();
    ZPicture.ZRFPMSReport();

    ZPicture.ZRFPMSIndexStats(0,stdout);

/*    if (ZPicture.zclearFile()!=ZS_SUCCESS)// test clear file
                ZRFException.exit_abort();

*/

   ZPicture.zclose();


   wTimer.end();
    fprintf (stdout,
             " loading time %s \n",
             wTimer.reportDeltaTime().toString());

    ZPicture.zopen(ZRF_Read);
    printIndexKey_0(ZPicture);
    ZPicture.zclose();


   ZRandomFile::zheaderDump("/home/gerard/testindexedpictures.zmf");  //! using static features

   if (ZRandomFile::ZRFstat("/home/gerard/testindexedpictures.zmf")!=ZS_SUCCESS)
                                                ZRFException.exit_abort();

   ZRandomFile::zsurfaceScan("/home/gerard/testindexedpictures.zmf");

//!
//! search test
//!

   ZKey *wKey = ZPicture.createKeyValue(0);  //! Create a key structure for index number 0

   wKey->set<int>(155,0);
   wKey->setString("my heart beats",1);


   wKey->Dump(32,stdout);

 //  if (ZPicture.zgenerateKeyValue(wKey,0,2,155,wField1) !=ZS_SUCCESS)
 //                                               ZRFException.exit_abort();
   if (ZPicture.zsearch (wRecord,wKey)!=ZS_FOUND)
            ZRFException.exit_abort();
    return (0);
//!
//! delete 3 adjacent records
//!
   fprintf (stdout,"\n\n---------------------------------\n"
            "                 remove test\n");

   ZPicture.zopen("testpictures",ZRF_Write);

   ZBlockDescriptor wBD;

   if (ZPicture.zgetBlockDescriptor(2L,wBD)!=ZS_SUCCESS)
                                ZRFException.exit_abort();


   fprintf (stdout,"=============================removing record #4 address %lld block size %lld\n", wBD.Address, wBD.BlockSize);

   wTimer.start();

   if (ZPicture.zremove(4)!=ZS_SUCCESS)
                ZRFException.exit_abort();

   wTimer.end();
    fprintf (stdout,
             " removing time %s \n",
             wTimer.reportDeltaTime().toString());


        ZRandomFile::zsurfaceScan("testpictures");



   if (ZPicture.zgetBlockDescriptor(2L,wBD)!=ZS_SUCCESS)
                                ZRFException.exit_abort();


   fprintf (stdout,"\n===============================removing record #3 address %lld block size %lld\n", wBD.Address, wBD.BlockSize);
   wTimer.start();
   if (ZPicture.zremove(3)!=ZS_SUCCESS)
                ZRFException.exit_abort();
   wTimer.end();
    fprintf (stdout,
             " removing time %s \n",
             wTimer.reportDeltaTime().toString());

         ZRandomFile::zsurfaceScan("testpictures");

   if (ZPicture.zgetBlockDescriptor(2L,wBD)!=ZS_SUCCESS)
                                ZRFException.exit_abort();


   fprintf (stdout,"\n===============================removing record #2 address %lld block size %lld\n", wBD.Address, wBD.BlockSize);

   if (ZPicture.zremove(2)!=ZS_SUCCESS)
                ZRFException.exit_abort();


//   if (ZRandomFile::_ZRFstat("testpictures")!=ZS_SUCCESS)
//                                                ZRFException.exit_abort();
   ZRandomFile::zsurfaceScan("testpictures");

#ifdef __COMMENT__

   fprintf (stdout,"\n===============================recovering free pool block  #0\n");

   long wIndex;
   if ((wSt=ZPicture.zrecover(0L,wIndex))!=ZS_SUCCESS) //! load document
               {
                   ZRFException.exit_abort();
               }

   ZRandomFile::zsurfaceScan("testpictures");
#endif // __COMMENT__
//!
//! Add 3 more records  from Table #6  (huge one)  #10 #11
//!
   fprintf (stdout,"\n\n---------------------------------\n"
            "                 add test\n");

   wTimer.start();


   wRecord.setData( &T1Cst[6],sizeof(T1));  //! get the occurrence of T1
   wURIPicture = PictureFiles[6];         //! set document path
   if ((wSt=wURIPicture.loadContent(wPicture))!=ZS_SUCCESS) //! load document
                                                ZException.exit_abort();

   wRecord += wPicture;                    //! append document content to record buffer
   if ((wSt=ZPicture.zadd(wRecord))!=ZS_SUCCESS) //! load document

    wTimer.end();
   fprintf (stdout,
            " adding time %s \n",
                wTimer.reportDeltaTime().toString());


   ZRandomFile::zsurfaceScan("testpictures");
   for (int wi = 10;wi < 12;wi++)
        {
            wRecord.setData( &T1Cst[wi],sizeof(T1));  //! get the occurrence of T1
            wURIPicture = PictureFiles[wi];         //! set document path
            if ((wSt=wURIPicture.loadContent(wPicture))!=ZS_SUCCESS) //! load document
                        {
                            ZException.exit_abort();
                        }

            wRecord += wPicture;                    //! append document content to record buffer
            if ((wSt=ZPicture.zadd(wRecord))!=ZS_SUCCESS) //! load document
                        {
                            ZRFException.exit_abort();
                        }
        }

   ZPicture.zclose();

      ZRandomFile::zsurfaceScan("testpictures");


//!
//! let's add some rows into ZAM1 and let's see what happens to the dynamic join
//!
/*
    struct T1 T1Cst[] =
    {
    {"my heart beats",                 1505 },           // multiple correspondances in both ZIX
    {"my heart beats no more",          104},           // no correspondance in any
    {"xxxx",                            11011},          // with one correspondance
    {"xxxx",                            11012},          // many
    {"xxxx",                            11013},          // many times
    {"my heart beats",                 1506 },           // many to many
    {"jkfd",                            2025},                  // row that have been rejected
    {"jkfdlsakfj",                       2025}                  // row that have been rejected In ZAM1 to be inserted in ZAM2
};
*/
    struct T2 T4Cst[] =
    {
    {"my heart beats",                 1505 },           // multiple correspondances in both ZIX
    {"my heart beats no more",          104},           // no correspondance in any
    {"xxxx",                            11011},          // with one correspondance
    {"xxxx",                            11012},          // many
    {"xxxx",                            11013},          // many times
    {"my heart beats",                 1506 },           // many to many
    {"jkfd",                            2025},                  // row that have been rejected
    {"jkfdlsakfj",                       2025}                  // row that have been rejected In ZAM1 to be inserted in ZAM2
};


#ifdef __COMMENT__

//! Declare the CUF (Commit User Function)
//!
    char wCUFBuf [50] = "This is commit Data";
     wSt=ZAM1.declareCommitUserFunction( &CUFunction,(void *)&wCUFBuf);

    wSt=ZAM1.setJournaling(zbs::ZJAutoCommit);  //! for getting response within CUF quickly

//! push 3 new constants and insert one

    struct T1 T3Cst[] =
    {
    {"my heart does not beat",          1505 },         // to be pushed
    {"my heart beats no more",          104},           // to be pushed
    {"and so does not beat my skin",    1512} ,         // to be pushed
    {"oops",                            11011} };       // to be inserted at position 3

    wSt= ZAM1<<(T1&)T2Cst[0];
    printf ("push %ld  %s\n",T2Cst[0].AtomicField, T2Cst[0].String1);

    ZAM1<<(T1&)T2Cst[1];
    printf ("push %ld  %s\n",T2Cst[1].AtomicField, T2Cst[1].String1);
    ZAM1<<(T1&)T2Cst[2];
    printf ("push %ld  %s\n",T2Cst[2].AtomicField, T2Cst[2].String1);

    const T1 Key1_coll = {"xxxx",                    110} ;
/*    long st = ZAM1.insert((T1&)Key1_coll,1);
*/

     void * wZIXptr= ZAM1.askZIndex(0,sizeof(T1::AtomicField));

     zbs::ZIndex<T1,K1_Bulk,char*,zbs::stringCMP<char*> > *ZIXptr=static_cast<zbs::ZIndex<T1,K1_Bulk,char*,zbs::stringCMP<char*> >*>(wZIXptr);





//    wSt=ZAM1.erase (1025); // nope
//    wSt=ZI2.remove(999);
    wi=3;
    printf ("%4ld>> AtomicField <%6ld> String1 <%s> \n",
            wi,
            ZI1[wi].AtomicField,        //! direct read of the ZAM fields from ZIndex
            ZI1[wi].String1);

    wSt=ZAM1.replace((T1&)Key1_coll,2);

    ZAM1.printJournal();
    ZAM1.printHistory();

    printf ("ZAM1 content ---------\n");
    for (long wi=0;wi< ZAM1.size();wi++)
                {
                printf ("%3ld>> AtomicField <%ld> String1 <%s>\n",
                        wi,
                        ZAM1[wi].AtomicField ,
                        ZAM1[wi].String1
                        );
                }
    printf ("Dump Index ZI2 content ---------\n");
    for (long wi=0;wi< ZI1.size();wi++)
                {
                printf ("%3ld>> ZAMIndex <%4ld>  ZAMState <%s> Key <%ld>\n",
                        wi,
                        ZI2.Tab[wi].ZAMIdx ,
                        decode_ZAMState(ZI2.Tab[wi].State),
                        ZI2.Tab[wi]._Key
                        );
                }
    printf ("----------ZI2 list before erase(2,1)-----------\n");
    for (long wi=0;wi< ZI2.size();wi++)
                {
                printf ("%4ld>> AtomicField <%6ld> String1 <%s> \n",
                        wi,
                        ZI2[wi].AtomicField,
                        ZI2[wi].String1);
                }
    printf ("------------->erase(2,1)   AtomicField <%ld>  String1 <%s>\n",
            ZAM1[2].AtomicField,
            ZAM1[2].String1);

    ZAM1.erase(2,1);   // the third is bad : let's suppress it  we can use also ZAM1.erase(2); it is quicker
    printf ("Dump  ZI2.ZReverse content ---------\n");

    printf ("Dump Index ZI2 content ---------\n");
    for (long wi=0;wi< ZI1.size();wi++)
                {
                printf ("%3ld>> ZAMIndex <%4ld>  ZAMState <%s> Key <%s>\n",
                        wi,
                        ZI2.Tab[wi].ZAMIdx ,
                        decode_ZAMState(ZI2.Tab[wi].State),
                        ZI2.printKeyValue(wi,wBuf)
                        );
                }
    printf ("----------ZI2 list after erase(2,1)-----------\n");
    for (long wi=0;wi< ZI2.size();wi++)
                {
                printf ("%4ld>> AtomicField <%6ld> String1 <%s> \n",
                        wi,
                        ZI2[wi].AtomicField,
                        ZI2[wi].String1);
                }
    printf ("------------->insert 3rd constant à rank 3  AtomicField <%ld> String1 <%s> \n",
            T2Cst[3].AtomicField,T2Cst[3].String1);

    ZAM1.insert((T1&)T2Cst[3],3); // insert the 3rd constant at rank 3  (within ZAM1)
    printf ("\n\nZAM1 elements in raw ZAM order \n");
    for (long wi=0;wi<ZAM1.size();wi++)
                fprintf (stdout,"%4ld>> AtomicField <%6ld> String1 <%s>\n",
                            wi,
                            ZAM1[wi].AtomicField,
                            ZAM1[wi].String1);




//!--------ZAM Journaling facilities--------------
//!
//! lets see what contains ZJournaling log
//!

    ZAM1.printJournal(-1,       //! (default:-1) -1 for all journal indexes (or specify a positive number for index)
                       true,    //! (default:false) true means 'include dump' of journalized row content in print out
                       stdout);  //! (default:stdout) what is the output chanel (could be something else)

//! OOps we made a mistake the last 3 operations
//! lets rollback
//!
//!  if the ZAMmark is positive : then search the effective operation number within the journal
//!  it ZAMmark is negative : this comes to roll back the last mentionned
//!
//!
//!
//!


    printf ("Dump Index ZI1 content ---------\n");
    for (long wi=0;wi< ZI1.size();wi++)
                {
                printf ("%3ld>> ZAMIndex <%4ld>  ZAMState <%s> Key <%s>\n",
                        wi,
                        ZI1.Tab[wi].ZAMIdx ,
                        decode_ZAMState(ZI1.Tab[wi].State),
                         ZI1.Tab[wi]._Key
                        );
                }

    printf ("Dump Index ZI2 content ---------\n");
    for (long wi=0;wi< ZI2.size();wi++)
                {
                printf ("%3ld>> ZAMIndex <%4ld>  ZAMState <%s> Key <%ld>\n",
                        wi,
                        ZI2.Tab[wi].ZAMIdx ,
                        decode_ZAMState(ZI2.Tab[wi].State),
                        ZI2.Tab[wi]._Key
                        );
                }
    //!             Rollback syntax
    //!
    //! rollback ();        means : rollback all available journal events
    //! rollback (-n) ;     means : rollback the last n elements starting at the last element until last -n
    //! rollback (m) ;      means : rollback all events until journal rank m

    ZAM1.rollback(-3); //! lets take for roll back the last 3 operations


    printf ("Dump Index ZI2 content ---------\n");
    for (long wi=0;wi< ZI1.size();wi++)
                {
                printf ("%3ld>> ZAMIndex <%4ld>  ZAMState <%s> Key <%ld>\n",
                        wi,
                        ZI2.Tab[wi].ZAMIdx ,
                        decode_ZAMState(ZI2.Tab[wi].State),
                        ZI2.Tab[wi]._Key
                        );
                }
    printf ("----------ZI2 list -----------\n");
    for (long wi=0;wi< ZI2.size();wi++)
                {
                printf ("%4ld>> AtomicField <%6ld> String1 <%s> \n",
                        wi,
                        ZI2[wi].AtomicField,
                        ZI2[wi].String1);
                }

    printf (" pop ZAM1 for rank <%ld> <%ld> <%s>\n",
            ZAM1.lastIdx(),
            ZAM1[ZAM1.lastIdx()].AtomicField,
            ZAM1[ZAM1.lastIdx()].String1);


    ZAM1.printJournal();
    ZAM1.printHistory();
    ZAM1.pop();             //suppress the last element

    printf ("Dump  ZI2.ZReverse content ---------\n");

    printf ("Dump Index ZI2 content ---------\n");
    for (long wi=0;wi< ZI1.size();wi++)
                {
                printf ("%3ld>> ZAMIndex <%4ld>  ZAMState <%s> Key <%ld>\n",
                        wi,
                        ZI2.Tab[wi].ZAMIdx ,
                        decode_ZAMState(ZI2.Tab[wi].State),
                        ZI2.Tab[wi]._Key
                        );
                }
    printf ("----------ZI2 list -----------\n");
    for (long wi=0;wi< ZI2.size();wi++)
                {
                printf ("%4ld>> AtomicField <%6ld> String1 <%s> \n",
                        wi,
                        ZI2[wi].AtomicField,
                        ZI2[wi].String1);
                }
    printf (" ------------------pop_front ZAM1  <%ld> <%s>\n",
            ZAM1[0].AtomicField,
            ZAM1[0].String1);

    ZAM1.pop_front();       // suppress the first ZAM event

    printf ("----------ZI2 list after pop_front()-----------\n");

    printf ("Dump Index ZI2 content ---------\n");
    for (long wi=0;wi< ZI1.size();wi++)
                {
                printf ("%3ld>> ZAMIndex <%4ld>  ZAMState <%s> Key <%ld>\n",
                        wi,
                        ZI2.Tab[wi].ZAMIdx ,
                        decode_ZAMState(ZI2.Tab[wi].State),
                        ZI2.Tab[wi]._Key
                        );
                }
    printf("ZAM1 ordered ZI2\n");
    for (long wi=0;wi< ZI2.size();wi++)
                {
                printf ("%4ld>> AtomicField <%6ld> String1 <%s> \n",
                        wi,
                        ZI2[wi].AtomicField,
                        ZI2[wi].String1);
                }


    //! search for ZI2 key value 104
    //!

    zbs::ZResult wZRes= ZI2.seek(12045);

    if (wZRes.ZSt!=ZS_FOUND)
                {
            printf ("Key 12045 not found on ZI2 status %s\n",decode_ZStatus(wZRes.ZSt));
                }

//    ZAM1[wZRes.ZIdx].AtomicField = 20786; // false cannot modify ZAM like that (read only values)

    T1 Rep ;
    Rep.AtomicField= ZAM1[wZRes.ZIdx].AtomicField;
    Rep.String1 = ZAM1[wZRes.ZIdx].String1;

    Rep.AtomicField = 20786 ;
    ZAM1.replace(Rep,wZRes.ZIdx);   // then replace.

    printf ("----------ZI2 list after replace with <20786>-----------\n");


    printf ("Dump Index ZAM1 content ---------\n");
    for (long wi=0;wi< ZI2.size();wi++)
                {
                printf ("%3ld>> AtomicField <%ld> String1 <%s>\n",
                        wi,
                        ZAM1[wi].AtomicField ,
                        ZAM1[wi].String1
                        );
                }


    //! lets print again what contains ZJournaling log
    //!

        ZAM1.printJournal(-1,       //! (default:-1) -1 for all journal indexes (or specify a positive number for index)
                           false,    //! (default:false) true means 'include dump' of journalized row content in print out
                           stdout);  //! (default:stdout) what is the output chanel (could be something else)


//!        That's correct, let' commit all
//!
//!
        ZAM1.commit(nullptr,-1);  //! nullptr : no user routine - -1 : commit all events

        ZAM1.printJournal();  //! lets print again the journal log

        //! see what's in history
        ZAM1.printHistory(-1,       //! same as printJournal for Historized events
                          false,
                          stdout);




    /**     --Dynamic Join  Test Plan--
     *  Overload of [] operator for accessing dynamic join content
     *
     * 1. Master side
     *  adding row within Master with multiple correspondance within Slave ZKA
     *  add row within Master with one correspondance within Slave
     *  add row within Master with NO correspondance within Slave
     *
     * 2. Slave side
     *  add row within Slave
     *  add row within Slave with multiple correspondance within Master
     *  add row within Slave with no correspondance within Master
     *
     * 3. Erase
     *  erase row within Master with only one correspondance within Slave
     *  erase row within Master with multiple correspondances within Slave
     *  erase row within Slave with multiple correspondance within Master
     *  erase multiple rows within Master
     *  erase multiple rows within Slave
     *
     * 4.change
     *  change row within Master
     *  change row witnin Slave
     *  modify row Master using = operator on _Key value (error case)
     *  modify row Master using = operator on other field than _Key value
     *  modify row Slave using = operator on _Key value (error case)
     *  modify row Slave using = operator on other field than _Key value
     *
     *
     */

#endif

_RETURN_ (0);


} // main

