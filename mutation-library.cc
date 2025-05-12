//
// mutation-library.cc built for fuzzing PDF.
// 
// Author: Erik Viken
//

#include "afl-fuzz.h"
//#include "afl-mutations.h"
#include "afl-mutations-modified.h"
#include "mutation-library.h"

#undef R // qpdf/QPDF.hh uses R, so it cannot be defined

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <string>
#include <cstring>
#include <sstream>
#include <cctype>

#include <qpdf/QPDF.hh>
#include <qpdf/Pl_Buffer.hh>
#include <qpdf/Constants.h>
#include <qpdf/BufferInputSource.hh>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "cppcodec/base64_rfc4648.hpp"

// --------------------------
// Variables
// --------------------------

//
// pdf_keys taken from AFLplusplus/dictionaries/pdf.dict
//
// Created by Ben Nagy <ben@iagu.net>
//

std::string pdf_keys[] = {
"/",
"/#23clipboard",
"/.notdef",
"/1",
"/1.0",
"/1.3",
"/3D",
"/3DA",
"/3DAnimationStyle",
"/3DB",
"/3DD",
"/3DI",
"/3DLightingScheme",
"/3DRenderMode",
"/3DV",
"/3DView",
"/90pv-RKSJ-H",
"/A",
"/A0",
"/A85",
"/AA",
"/AAIC",
"/AAPL",
"/ABCDEF+ACaslonPro-Regular",
"/ABCDEF+AJensonPro-LtIt",
"/ABCDEF+AdobeCorpID-MinionRg",
"/ABCDEF+Arial,Bold",
"/ABCDEF+BankGothicMdBT",
"/ABCDEF+Bauhaus-Heavy",
"/ABCDEF+BluesClues",
"/ABCDEF+BodegaSans",
"/ABCDEF+BodoniMTCondensed",
"/ABCDEF+BookAntiqua",
"/ABCDEF+CMBX10",
"/ABCDEF+CaflischScriptPro-Regular",
"/ABCDEF+CityBlueprint",
"/ABCDEF+CourierNewPSMT",
"/ABCDEF+FixedsysExcelsior2.00",
"/ABCDEF+MSTT31854bd45bo188067S00",
"/ABCDEF+MinionPro-BoldCnIt",
"/ABCDEF+MyriadMM-It_400_300_",
"/ABCDEF+Wingdings",
"/ABCDEF+ZapfDingbats",
"/AC",
"/ADBE",
"/ADB_DEVICE_DEFAULT_STYLE",
"/ADB_DefaultStyle",
"/ADB_NO_TRAP_STYLE",
"/AE",
"/AESV2",
"/AGaramond",
"/AH",
"/AI8DstIndex",
"/AI8SrcIndex",
"/AIMetaData",
"/AIPDFPrivateData1",
"/AIS",
"/AL",
"/AN",
"/AP",
"/AS",
"/ASCII85Decode",
"/ASCIIHexDecode",
"/ASomewhatLongerName",
"/AU",
"/Aacute",
"/Acc.#20Prod.#202501#20#2F2#20#20",
"/Accounts#20payable",
"/AccurateScreens",
"/Acircumflex",
"/AcroForm",
"/Action",
"/Actual",
"/Add",
"/Adieresis",
"/Adobe",
"/Adobe#20PDF#20Library",
"/Adobe.PPKLite",
"/AdobeCorpID-Acrobat",
"/AdobeCorpID-MinionRg",
"/AdobePhotoshop",
"/Agrave",
"/All",
"/AllKO",
"/AllOn",
"/Alt",
"/Alternate",
"/AlternatePresentations",
"/Alternates",
"/Amex",
"/And",
"/Angle",
"/Annot",
"/Annots",
"/AntiAlias",
"/AnyOn",
"/Apag_PDFX_Checkup",
"/App",
"/Architecture-Normal",
"/Arial",
"/Aring",
"/Art",
"/ArtBox",
"/Article",
"/Artifact",
"/Artwork",
"/Ascent",
"/Aspect",
"/Assistant",
"/Atilde",
"/AuthEvent",
"/Author",
"/Avenir-Heavy",
"/Avenir-MediumOblique",
"/AvgWidth",
"/BBox",
"/BC",
"/BCL",
"/BDC",
"/BDL",
"/BE",
"/BFSOL",
"/BG",
"/BG2",
"/BM",
"/BMC",
"/BS",
"/BW",
"/Bank",
"/BaseEncoding",
"/BaseFont",
"/BaseState",
"/BaseVersion",
"/Birch",
"/BitsPerComponent",
"/BitsPerCoordinate",
"/BitsPerFlag",
"/BitsPerSample",
"/Bl",
"/BlCDel",
"/BlMiNu",
"/Black",
"/BlackIs1",
"/BlackOP",
"/BlackPoint",
"/BleedBox",
"/Blend",
"/Block",
"/Blue",
"/BluesClues",
"/Bookshelf",
"/Border",
"/Bounds",
"/BoxColorInfo",
"/Btn",
"/BulmerMT-BoldDisplay",
"/ByteRange",
"/C",
"/C0",
"/C0_0",
"/C1",
"/C2W",
"/C3",
"/CALS_AIS",
"/CALS_BM",
"/CALS_HT",
"/CALS_SMASK",
"/CALS_ca",
"/CAM",
"/CB",
"/CC",
"/CCH",
"/CCITTFaxDecode",
"/CD",
"/CDL",
"/CEN",
"/CF",
"/CFM",
"/CI",
"/CIDFontType0",
"/CIDFontType0C",
"/CIDFontType2",
"/CIDInit",
"/CIDSet",
"/CIDSystemInfo",
"/CIDToGIDMap",
"/CMV_LabBar",
"/CMV_LabControl",
"/CMYK",
"/CMYK#20#2880,#208,#2034,#200#29",
"/CMap",
"/CMapName",
"/CMapType",
"/CMapVersion",
"/CO",
"/CP",
"/CS",
"/CS0",
"/CT",
"/CV",
"/CalGray",
"/CalRGB",
"/CapHeight",
"/Caption",
"/Caslon540BT-Roman",
"/CaslonBT-Bold",
"/CaslonBT-BoldItalic",
"/Catalog",
"/Category",
"/Ccedilla",
"/CenturySchoolbookBT-Roman",
"/Ch",
"/Chair",
"/Chap",
"/Chaparral-Display",
"/CharProcs",
"/CharSet",
"/CheckSum",
"/Circle",
"/ClarendonBT-Black",
"/ClassMap",
"/Clearface-Black",
"/Clip",
"/ClippedText",
"/Cn",
"/Collection",
"/CollectionItem",
"/CollectionSchema",
"/CollectionSubitem",
"/Color",
"/ColorBurn",
"/ColorDodge",
"/ColorMatch",
"/ColorSpace",
"/ColorTransform",
"/ColorType",
"/Colorants",
"/Colors",
"/Columns",
"/ComicSansMS,Bold",
"/Comment",
"/Comments",
"/Company",
"/Compatibility",
"/Compatible",
"/Components",
"/CompressArt",
"/Condensed",
"/Configs",
"/Consultant",
"/ContainerVersion",
"/Contents",
"/Coords",
"/Copy",
"/Copy#20center",
"/Cor",
"/Corner#20surface",
"/CosineDot",
"/Count",
"/Cour",
"/Courier",
"/Create",
"/CreationDate",
"/Creator",
"/CreatorInfo",
"/CreatorVersion",
"/CropBox",
"/CropFixed",
"/CropRect",
"/Crypt",
"/CryptFilter",
"/CryptFilterDecodeParms",
"/Cs12",
"/Cs3",
"/Cyan",
"/D",
"/DA",
"/DCTDecode",
"/DIC#202525p*",
"/DIS",
"/DL",
"/DOS",
"/DP",
"/DR",
"/DS",
"/DSz",
"/DV",
"/DW",
"/DamagedRowsBeforeError",
"/Darken",
"/Data",
"/Date",
"/Decode",
"/DecodeParms",
"/DefEmbeddedFile",
"/Default",
"/DefaultCryptFilter",
"/DefaultForPrinting",
"/DefaultRGB",
"/Delete",
"/Delta",
"/DescendantFonts",
"/Descent",
"/Description",
"/Design",
"/Dest",
"/DestOutputProfile",
"/DestOutputProfileRef",
"/Dests",
"/DeviceCMYK",
"/DeviceGray",
"/DeviceN",
"/DeviceRGB",
"/Difference",
"/Differences",
"/DigestLocation",
"/DigestMethod",
"/DigestValue",
"/Dimmed",
"/Direction",
"/DisplayDocTitle",
"/Dissolve",
"/Div",
"/Dm",
"/DocMDP",
"/DocOpen",
"/Document",
"/Documents",
"/Domain",
"/Door",
"/DotGain",
"/Draw",
"/Dt",
"/Dur",
"/Dynamic#20connector",
"/E",
"/EF",
"/EFF",
"/EMC",
"/Eacute",
"/EarlyChange",
"/Ecircumflex",
"/Edieresis",
"/Editable",
"/Egrave",
"/EmbedFonts",
"/EmbedICCProfile",
"/Embedded",
"/EmbeddedFile",
"/EmbeddedFiles",
"/Encode",
"/EncodedByteAlign",
"/Encoding",
"/Encrypt",
"/EncryptMetadata",
"/EndIndent",
"/EndOfBlock",
"/EndOfLine",
"/Euro",
"/Euro.037",
"/Event",
"/ExData",
"/Exchange-Pro",
"/Exclude",
"/Exclusion",
"/Executive",
"/Export",
"/ExportCrispy",
"/ExportState",
"/ExtGState",
"/Extend",
"/Extends",
"/ExtensionLevel",
"/Extensions",
"/F",
"/F1",
"/F1.0",
"/F12",
"/F13",
"/F3",
"/F5",
"/F6",
"/F7",
"/F8",
"/FB",
"/FD",
"/FDecodeParms",
"/FFilter",
"/FICL",
"/FM",
"/FOV",
"/FRM",
"/FS",
"/FT",
"/Facilities",
"/Fade",
"/False",
"/Feature",
"/FedEx#20Orange",
"/FedEx#20Purple",
"/Field",
"/Fields",
"/Figure",
"/File",
"/Files",
"/Filespec",
"/FillIn",
"/Filter",
"/First",
"/FirstChar",
"/FirstPage",
"/Fit",
"/FitB",
"/FitBH",
"/FitBV",
"/FitH",
"/FitR",
"/FitV",
"/FitWindow",
"/FixedPrint",
"/Flags",
"/FlateDecode",
"/Fm0",
"/Fm4",
"/Fo",
"/Focoltone#201047",
"/Font",
"/FontBBox",
"/FontDescriptor",
"/FontFamily",
"/FontFile",
"/FontFile2",
"/FontMatrix",
"/FontName",
"/FontStretch",
"/FontWeight",
"/Form",
"/FormEx",
"/FormType",
"/FreeText",
"/FreeTextCallout",
"/Frequency",
"/FullSave",
"/FullScreen",
"/Function",
"/FunctionType",
"/Functions",
"/Futura-Bold",
"/Futura-CondensedExtraBold",
"/G",
"/G02",
"/GLGR",
"/GS0",
"/GS1",
"/GS2",
"/GTS",
"/GTS_PDFA1",
"/GTS_PDFX",
"/GTS_PDFXConformance",
"/GTS_PDFXVersion",
"/GWG#20Green",
"/Gamma",
"/Garamond",
"/Georgia,Bold",
"/GoTo",
"/GoTo3DView",
"/GoToE",
"/GoToR",
"/Gold",
"/Goudy",
"/Gray",
"/Green",
"/GreymantleMVB",
"/GrotesqueMT",
"/Group",
"/H",
"/HDAG_Tools",
"/HKana",
"/HT",
"/HT2",
"/Halftone",
"/HalftoneName",
"/HalftoneType",
"/HardLight",
"/HeBo",
"/Head1",
"/Headlamp",
"/Height",
"/HeiseiMin",
"/Helv",
"/Helvetica",
"/Helvetica-Bold",
"/Helvetica-BoldOblique",
"/Helvetica-Condensed",
"/HelveticaNeue-Black",
"/Hide",
"/HonMincho-M",
"/Horizontal",
"/Hue",
"/I",
"/I0",
"/IC",
"/ICCBased",
"/ICCVersion",
"/ID",
"/IDS",
"/IDTree",
"/IEC",
"/IF",
"/IN",
"/ISO32000Registry",
"/ISO_PDFE1",
"/ISO_PDFEVersion",
"/IT",
"/ITO",
"/ITP",
"/IV",
"/IX",
"/Icircumflex",
"/Icon",
"/Identity",
"/Identity-H",
"/IgnEP",
"/Illustrator",
"/Illustrator8.0",
"/Im0",
"/Im1",
"/Im2",
"/Im3",
"/Im4",
"/Image",
"/Image1",
"/ImageB",
"/ImageC",
"/ImageI",
"/ImageMask",
"/ImageResources",
"/ImageType",
"/Import",
"/ImportData",
"/ImpressBT-Regular",
"/Index",
"/Indexed",
"/Info",
"/Information#20services",
"/Ink",
"/InkList",
"/InsertPages",
"/Insignia",
"/IntegerItem",
"/Intent",
"/Interpolate",
"/ItalicAngle",
"/ItcKabel-Ultra",
"/Item1",
"/Item2",
"/JBIG2Decode",
"/JBIG2Globals",
"/JPXDecode",
"/JS",
"/JT",
"/JTC",
"/JTF",
"/JTFile",
"/JTM",
"/JavaScript",
"/JobTicketContents",
"/Justify",
"/K",
"/Keywords",
"/Kids",
"/L",
"/L1",
"/L1a",
"/L1b",
"/L2R",
"/L50188",
"/LBody",
"/LI",
"/LL",
"/LLE",
"/LLO",
"/LS",
"/LSP",
"/LZW",
"/LZWDecode",
"/Lab",
"/Lang",
"/Last",
"/LastChar",
"/LastItem",
"/LastModified",
"/Lateral#20file",
"/Launch",
"/Layout",
"/Lbl",
"/Leading",
"/Legal",
"/Length",
"/Length1",
"/Length2",
"/Length3",
"/LetterspaceFlags",
"/Lighten",
"/Limits",
"/Line",
"/LineDimension",
"/LineHeight",
"/Linear",
"/Linearized",
"/Link",
"/Locked",
"/LogoGreen",
"/LrTb",
"/Lslash",
"/Luminosity",
"/M",
"/MB",
"/MC",
"/MC0",
"/MCD",
"/MCID",
"/MCR",
"/MD5",
"/MH",
"/MIT",
"/MK",
"/MMType1",
"/MP",
"/MR",
"/MS",
"/MUX#20#2F#20DEMUX",
"/Mac",
"/MacRomanEncoding",
"/Magenta",
"/Manager",
"/MarkInfo",
"/Marked",
"/MarkedPDF",
"/Marker#20board",
"/Markup3D",
"/Mask",
"/Mastercard",
"/Matrix",
"/Max",
"/MaxLen",
"/MaxWidth",
"/Me",
"/Measure",
"/MediaBox",
"/MetaData",
"/Min",
"/MinionMM",
"/MissingWidth",
"/MixedContainer",
"/MixingHints",
"/ModDate",
"/Mode",
"/Modify",
"/Movie",
"/Msg",
"/MurrayHillBT-Bold",
"/MxGeom",
"/MxLaNu",
"/MxPts",
"/MyriadPro-Black",
"/NA",
"/NChannel",
"/ND",
"/NL",
"/NM",
"/NR",
"/Name",
"/Name1",
"/Named",
"/Names",
"/NeedsRendering",
"/NewCenturySchlbk-Italic",
"/NewWindow",
"/Next",
"/NextPage",
"/No",
"/NonEFontNoWarn",
"/NonStruct",
"/None",
"/Normal",
"/Not",
"/NotDefSpecial",
"/NumBlock",
"/Nums",
"/OB",
"/OBJR",
"/OC",
"/OC2",
"/OC3",
"/OC4",
"/OCG",
"/OCGs",
"/OCL",
"/OCMD",
"/OCProperties",
"/OE",
"/OFF",
"/OLN",
"/ON",
"/OOL",
"/OPBG",
"/OPBS",
"/OPI",
"/OPM",
"/OS",
"/OT",
"/Oacute",
"/Obj",
"/ObjStm",
"/Ocircumflex",
"/Odieresis",
"/Ograve",
"/Omega",
"/OneColumn",
"/Online",
"/Open",
"/OpenAction",
"/Operation",
"/Opt",
"/OptionSet",
"/Options",
"/Or",
"/Orange",
"/Order",
"/Ordering",
"/OriginalLayerName",
"/Oslash",
"/Otilde",
"/Outlines",
"/OutputCondition",
"/OutputConditionIdentifier",
"/OutputIntent",
"/OutputIntents",
"/Overlay",
"/P0",
"/P1",
"/P2",
"/P2,#2300ff007900000000,PANTONE#20151#20C",
"/PANTONE",
"/PANTONE#20158-5#20CVS",
"/PANTONE#20221#20CVU",
"/PANTONE#203405#20C",
"/PANTONE#20399#20CVC",
"/PANTONE#20Blue#20072#20C",
"/PANTONE#20Orange#20021#20C",
"/PANTONE#20Orange#20021#20CVC",
"/PANTONE#20Yellow#20C",
"/PC",
"/PDFDocEncoding",
"/PIX",
"/PO",
"/PS",
"/PUBLISHER",
"/PZ",
"/Pa0",
"/Page",
"/PageElement",
"/PageLabels",
"/PageLayout",
"/PageMode",
"/PageRange",
"/Pages",
"/PaintType",
"/Palatino,Bold",
"/Pale#20Brown.c",
"/Panose",
"/Paper#20tray",
"/Para",
"/Params",
"/Parent",
"/ParentTree",
"/ParentTreeNextKey",
"/Part",
"/Pattern",
"/PatternType",
"/PcZ",
"/Perceptual",
"/Perms",
"/Pg",
"/Pgf",
"/PieceInfo",
"/PitStop",
"/Placement",
"/Play",
"/Polygon",
"/PolygonCloud",
"/Popup",
"/Position",
"/PowerUpPDF",
"/PrOut",
"/PrRGBGra",
"/PrRGBIma",
"/Predictor",
"/PresSteps",
"/PreserveRB",
"/Prev",
"/PrevPage",
"/Preview",
"/Print",
"/PrintRecord",
"/PrintScaling",
"/PrintState",
"/PrintStyle",
"/Printed",
"/PrintingOrder",
"/Private",
"/ProcSet",
"/Process",
"/ProcessBlue",
"/ProcessGreen",
"/ProcessRed",
"/Producer",
"/ProfileCS",
"/ProfileName",
"/Prop_Build",
"/Properties",
"/Proportional",
"/PubSec",
"/Q",
"/QuadPoints",
"/R1",
"/RBGroups",
"/RC",
"/RD",
"/REC",
"/REx",
"/RF",
"/RGB",
"/RI",
"/RICMYKGra",
"/RICMYKIma",
"/RICalGra",
"/RICalIma",
"/RIDefault",
"/RIDevNGra",
"/RIDevNIma",
"/RIRGBGra",
"/RIRGBIma",
"/RL",
"/RM",
"/RV",
"/Range",
"/Rect",
"/Red",
"/Redact",
"/Ref",
"/Reference",
"/Registry",
"/RegistryName",
"/RelativeColorimetric",
"/Rendition",
"/Renditions",
"/Requirements",
"/ResetForm",
"/Resolution",
"/Resources",
"/ReversedChars",
"/RoleMap",
"/Root",
"/Rotate",
"/Round",
"/RoundTrip",
"/RoundtripVersion",
"/Router",
"/Rows",
"/RunLengthDecode",
"/Ryumin",
"/SA",
"/SBDraft",
"/SC",
"/SE",
"/SFSSL",
"/SFTWS",
"/SI",
"/SL",
"/SLA",
"/SM",
"/SMask",
"/SMaskInData",
"/SP",
"/SPS",
"/STL",
"/SU",
"/SW",
"/Saturation",
"/SaveAs",
"/SaveContents",
"/SaveResource",
"/SavedBy",
"/Scaron",
"/Schema",
"/Screen",
"/Sect",
"/SemiCondensed",
"/SemiExpanded",
"/Separation",
"/SeparationInfo",
"/SetOCGState",
"/SettingsFileName",
"/Sh0",
"/Sh1",
"/Shading",
"/ShadingType",
"/Shape",
"/Sig",
"/SigFlags",
"/SigRef",
"/Signature",
"/Signed",
"/SinglePage",
"/Size",
"/SlideShow",
"/SoftLight",
"/Solid",
"/Solidities",
"/SomeName",
"/Sort",
"/Sound",
"/Space",
"/SpaceAfter",
"/SpaceBefore",
"/Span",
"/SpawnTemplate",
"/SpdrArt",
"/SpiderInfo",
"/Split",
"/Spot",
"/Spot1",
"/Spot2",
"/SpotFunction",
"/SpotMap",
"/St",
"/Stamp",
"/StandardImageFileData",
"/Star",
"/Start",
"/StartIndent",
"/StartResource",
"/State",
"/StdCF",
"/StemH",
"/StemV",
"/Stm",
"/StmF",
"/Stop",
"/Story",
"/StrF",
"/StrikeOut",
"/StringItem",
"/StructElem",
"/StructParent",
"/StructParents",
"/StructTreeRoot",
"/Style",
"/SubFilter",
"/SubType",
"/Subdictionary",
"/Subform",
"/Subj",
"/Subject",
"/SubmitForm",
"/SubmitStandalone",
"/SubsetFontsBelow",
"/SubsetFontsRatio",
"/Supplement",
"/Swiss721BT-Black",
"/Switch",
"/T",
"/T1",
"/T1_0",
"/TB",
"/TC",
"/TCS",
"/TF",
"/TID",
"/TK",
"/TM",
"/TO",
"/TOC",
"/TOCI",
"/TOYO#200004pc",
"/TP",
"/TR",
"/TR2",
"/TRUMATCH#206-e",
"/TS",
"/TSV",
"/TT",
"/TT0",
"/TTRefMan",
"/TU",
"/TV",
"/TW",
"/TWS",
"/TWY",
"/Tabs",
"/TagSuspect",
"/TargetCS",
"/Technical",
"/Template",
"/TemplateInstantiated",
"/Templates",
"/Text",
"/TextAlign",
"/TextBox",
"/TextIndent",
"/The",
"/This",
"/Thorn",
"/Thread",
"/Threads",
"/Thumb",
"/Thumbnail",
"/Thumbs",
"/Ti",
"/TiBI",
"/TilingType",
"/Times-BoldItalic",
"/Times-Roman",
"/Title",
"/ToUnicode",
"/Toggle",
"/Trans",
"/TransferFunction",
"/TransformMethod",
"/TransformParams",
"/Transparency",
"/TrapInfo",
"/TrapMagicNumber",
"/TrapRegions",
"/TrapSet",
"/Trapped",
"/Trapping",
"/TrappingDetails",
"/TrappingParameters",
"/TrimBox",
"/True",
"/TrueType",
"/TrustedMode",
"/TwoColumnLeft",
"/Tx",
"/Type",
"/Type0",
"/U3D",
"/UA",
"/UCR",
"/UCR2",
"/UIDOffset",
"/UR",
"/UR3",
"/URI",
"/URL",
"/URLs",
"/Uacute",
"/Ucircumflex",
"/Udieresis",
"/Ugrave",
"/Univers-BoldExt",
"/Unix",
"/Unknown",
"/Usage",
"/UseAttachments",
"/UseNone",
"/UseOC",
"/UseOutlines",
"/UseThumbs",
"/UsedCMYK",
"/UserProperties",
"/UserUnit",
"/V2",
"/VA",
"/VE",
"/VP",
"/Verdana,Bold",
"/Version",
"/Vertical",
"/VeryLastItem",
"/View",
"/ViewerPreferences",
"/Visa",
"/Visible",
"/Volume",
"/W",
"/W2",
"/WAI",
"/WAN",
"/WMode",
"/WP",
"/WarnockPro-BoldIt",
"/Watermark",
"/WebCapture",
"/Which",
"/WhiteBG",
"/WhitePoint",
"/Widget",
"/Width",
"/Widths",
"/Win",
"/WinAnsiEncoding",
"/Window",
"/Windows",
"/Work#20surface",
"/Workbook",
"/Worksheet",
"/WritingMode",
"/X",
"/X1",
"/XFA",
"/XHeight",
"/XML",
"/XN",
"/XObject",
"/XRef",
"/XRefStm",
"/XStep",
"/XUID",
"/XYZ",
"/Y",
"/YStep",
"/Yacute",
"/Ydieresis",
"/Yellow",
"/Z",
"/Z7KNXbN",
"/ZaDb",
"/ZapfDingbats",
"/Zcaron",
"/Zoom",
"/_No_paragraph_style_",
"/a1",
"/acute",
"/adbe.pkcs7.detached",
"/ampersand",
"/apple",
"/approxequal",
"/asciicircum",
"/asciitilde",
"/asterisk",
"/at",
"/audio#2Fmpeg",
"/b",
"/backslash",
"/bar",
"/blank",
"/braceleft",
"/braceright",
"/bracketleft",
"/bracketright",
"/breve",
"/brokenbar",
"/bullet",
"/c108",
"/cCompKind",
"/cCompQuality",
"/cCompression",
"/cRes",
"/cResolution",
"/ca",
"/caron",
"/cedilla",
"/cent",
"/circumflex",
"/colon",
"/comma",
"/copyright",
"/currency",
"/dagger",
"/daggerdbl",
"/degree",
"/deviceNumber",
"/dieresis",
"/divide",
"/dollar",
"/dotaccent",
"/dotlessi",
"/dotlessj",
"/eight",
"/ellipsis",
"/emdash",
"/endash",
"/equal",
"/eth",
"/exclam",
"/exclamdown",
"/f",
"/ff",
"/ffi",
"/ffl",
"/fi",
"/five",
"/fl",
"/florin",
"/four",
"/fraction",
"/gCompKind",
"/gCompQuality",
"/gCompression",
"/gRes",
"/gResolution",
"/germandbls",
"/go1",
"/grave",
"/greater",
"/greaterequal",
"/guillemotleft",
"/guillemotright",
"/guilsinglleft",
"/guilsinglright",
"/hungarumlaut",
"/hyphen",
"/iacute",
"/idieresis",
"/igrave",
"/infinity",
"/integral",
"/j",
"/k",
"/less",
"/lessequal",
"/logicalnot",
"/lozenge",
"/lt#20blue",
"/mCompKind",
"/mCompression",
"/mRes",
"/mResolution",
"/macron",
"/minus",
"/mu",
"/multiply",
"/n",
"/n0",
"/nine",
"/notequal",
"/ntilde",
"/numbersign",
"/o",
"/ogonek",
"/one",
"/onehalf",
"/onequarter",
"/onesuperior",
"/op",
"/ordfeminine",
"/ordmasculine",
"/p",
"/pageH",
"/pageV",
"/paragraph",
"/parenleft",
"/parenright",
"/partialdiff",
"/pdf",
"/pdfx",
"/percent",
"/period",
"/periodcentered",
"/perthousand",
"/pi",
"/plus",
"/plusminus",
"/pms#208400",
"/printX",
"/product",
"/question",
"/questiondown",
"/quotedbl",
"/quotedblbase",
"/quotedblleft",
"/quotedblright",
"/quoteleft",
"/quoteright",
"/quotesinglbase",
"/quotesingle",
"/r",
"/radical",
"/registered",
"/ring",
"/s",
"/s1",
"/sd1",
"/sd2",
"/section",
"/semicolon",
"/seven",
"/six",
"/slash",
"/sterling",
"/summation",
"/thinspace",
"/three",
"/threequarters",
"/threesuperior",
"/tilde",
"/trademark",
"/two",
"/twosuperior",
"/u",
"/underscore",
"/v",
"/w",
"/y1",
"/yen",
"/yes",
"/zero",
};

std::string stream_dict[] = {
"#",
"%",
"%%",
"%%EOF",
"%FDF-1.7",
"%PDF-1.7",
"(",
"(#)",
"($)",
"(/xdp:xdp)",
"(\\001)",
"(config)",
"(datasets)",
"(template)",
"(xdp:xdp)",
")",
"-1",
"-1.0",
"..",
"0 R",
"1",
"1.0",
"<",
"<<",
">",
">>",
"Adobe.PPKLite",
"Adobe.PubSec",
"B",
"B*",
"BDC",
"BI",
"BMC",
"BT",
"BX",
"CS",
"DP",
"Do",
"EI",
"EMC",
"ET",
"EX",
"Entrust.PPKEF",
"F",
"ID",
"J",
"K",
"M",
"MP",
"Q",
"R",
"RG",
"S",
"SC",
"SCN",
"T*",
"Tc",
"Td",
"TJ",
"TL",
"Tc",
"Td",
"Tf",
"Tj",
"Tm",
"Tr",
"Ts",
"Tw",
"W",
"W*",
"[",
"[0.0 0.0 0.0 0.0 0.0 0.0]",
"[1 1 1]",
"[1.0 -1.0 1.0 -1.0]",
"[1.0 -1.0]",
"\\",
"]",
"abs",
"adbe.pkcs7.s3",
"adbe.pkcs7.s4",
"adbe.pkcs7.s5",
"add",
"and",
"atan",
"begin",
"beginarrangedfont",
"beginbfchar",
"begincidrange",
"begincmap",
"begincodespacerange",
"beginnotdefchar",
"beginnotdefrange",
"beginusematrix",
"bitshift",
"c",
"ceiling",
"cm",
"copy",
"cos",
"cs",
"cvi",
"cvr",
"d0",
"d1",
"div",
"dup",
"end",
"endarrangedfont",
"endbfchar",
"endcidrange",
"endcmap",
"endcodespacerange",
"endnotdefchar",
"endnotdefrange",
"endobj",
"endstream",
"endusematrix",
"eq",
"exch",
"exp",
"f",
"f*",
"false",
"findresource",
"floor",
"g",
"ge",
"gs",
"gt",
"h",
"idiv",
"if",
"ifelse",
"index",
"k",
"l",
"le",
"ln",
"log",
"lt",
"m",
"mod",
"mul",
"n",
"ne",
"neg",
"not",
"null",
"obj",
"or",
"page",
"pop",
"q",
"re",
"rg",
"ri",
"roll",
"round",
"s",
"sc",
"sh",
"sin",
"sqrt",
"startxref",
"stream",
"sub",
"trailer",
"true",
"truncate",
"usecmap",
"usefont",
"v",
"xor",
"xref",
"y",
"{",
"}",
"\\92n",
"\\92"
};

char separators[] = {
'<',
'>',
' ',
'\n',
'\0'
};

// --------------------------
// Helper functions for debug
// --------------------------

void printHexData(const unsigned char* data, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    std::cout << std::hex << (data[i] >> 4) << (data[i] & 0x0F);
  }
  std::cout << std::endl;
}

// ----------------------------------------
// Helper functions
// ----------------------------------------

// mutation_chance
int mutation_chance(uint32_t mod){
    if(mod == 0) return 1;
    return (rand() % mod) == 0;
}

int randomness(my_mutator_t *data, uint32_t mod){
    return rand() % mod;
}

std::string get_random_key(){
    // Find random valid key
    size_t pdf_keys_len = sizeof(pdf_keys) / sizeof(pdf_keys[0]);
    std::string key = pdf_keys[rand() % pdf_keys_len];

    return key;
}

std::string get_stream_dict_value(){
    // Find random valid key
    size_t stream_dict_len = sizeof(stream_dict) / sizeof(stream_dict[0]);
    std::string dict_value = stream_dict[rand() % stream_dict_len];

    return dict_value;
}

uint32_t get_interesting_num(){
    size_t interesting_32_len = sizeof(interesting_32) >> 2;
    return interesting_32[rand() % interesting_32_len];
}

uint32_t get_mutated_integer(uint32_t original_num){

    uint32_t num = original_num;

    if (rand() % 4 > 0) {
        // Set interesting number
        num = get_interesting_num();
    }else{

        // Select number range
        int max_selection[] = {(1 << 2), (1 << 4), (1 << 8), (1 << 16), INT_MAX};
        uint32_t max_diff = max_selection[rand() % 5];

        // Add or subract num
        if (rand() % 2 > 0) {
            num += rand() % max_diff;
        }else{
            num -= rand() % max_diff;
        }

    }

    return num;
}

double get_mutated_double(double original_num) {
    
    // Pick random mutation
    int randChoice = rand() % 13;

    switch (randChoice) {
    case 0:
        return std::numeric_limits<double>::quiet_NaN();
    case 1:
        return std::numeric_limits<double>::min();
    case 2:
        return std::numeric_limits<double>::max();
    case 3:
        return -std::numeric_limits<double>::min();
    case 4:
        return -std::numeric_limits<double>::max();
    case 5:
        return std::numeric_limits<double>::epsilon();
    case 6:
        return -std::numeric_limits<double>::epsilon();
    case 7:
        return std::numeric_limits<double>::infinity();
    case 8:
        return -std::numeric_limits<double>::infinity();
    case 9:
        return 0.0;
    case 10: // Generate a random double between -1.0 and 1.0
        return -1.0 + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX) / 2.0);
    case 11:
        return original_num + static_cast<double>(rand());
    case 12:
        return original_num - static_cast<double>(rand());
    }
    return 0.0;
}

double get_safe_mutated_double(my_mutator_t *data, double original_num) {
    
    // Pick random mutation
    int randChoice = rand() % 12;

    switch (randChoice) {
    case 0:
        return std::numeric_limits<double>::min();
    case 1:
        return std::numeric_limits<double>::max();
    case 2:
        return -std::numeric_limits<double>::min();
    case 3:
        return -std::numeric_limits<double>::max();
    case 4:
        return std::numeric_limits<double>::epsilon();
    case 5:
        return -std::numeric_limits<double>::epsilon();
    case 6:
        return 0.0;
    case 7: // Generate a random double between -1.0 and 1.0
        return -1.0 + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX) / 2.0);
    case 8:
        return original_num + static_cast<double>(rand());
    case 9:
        return original_num - static_cast<double>(rand());
    case 10:
        return original_num + (-1.0 + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX) / 2.0));
    case 11:
        return original_num - (-1.0 + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX) / 2.0));
    }
    return 0.0;
}


std::string get_mutated_string(my_mutator_t *data, std::string mutate_string){

    // create mutated key
    std::string new_mutation;

    // Check if data is too big
    if(data->key_mutation_size <= mutate_string.size()){
      return mutate_string;
    }

    // Place decoded data into buf
    std::memcpy(data->key_mutation, mutate_string.data(), mutate_string.size());

    // Set havoc and explore
    u32 havoc_steps = randomness(data, 16) + 1;
    bool explore = randomness(data, 2) ? true : false;

    // Mutate the data with default afl++
    u32 out_buf_len = afl_mutate(data->afl, data->key_mutation, mutate_string.size(), havoc_steps, true, explore, NULL, 0, data->key_mutation_size);

    new_mutation = std::string(reinterpret_cast<char*>(data->key_mutation), out_buf_len);

    return new_mutation;

}


// ----------------------------------------
// Data checks
// ----------------------------------------

int is_trailer(rapidjson::Value& key){
  if (!key.IsNull() && key.IsString() && std::string(key.GetString()) == "trailer"){
    return 1;
  }
  return 0;
}

int is_string(rapidjson::Value& key){
  if (!key.IsNull() && key.IsString()){
    return 1;
  }
  return 0;
}

int is_value_replaceable(rapidjson::Value& key){

  if(key.IsNull()){
    return 1;
  }

  if(key.GetStringLength() >= 4){

    if(key.IsString() && strcmp(key.GetString(), "value") == 0){
      return 0;
    }

    if(key.IsString() && strcmp(key.GetString(), "stream") == 0){
      return 0;
    }

    if(key.IsString() && strcmp(key.GetString(), "data") == 0){
      return 0;
    }

    if(key.IsString() && strcmp(key.GetString(), "dict") == 0){
      return 0;
    }

    if(key.GetString()[0] == 'o' && key.GetString()[1] == 'b' && key.GetString()[2] == 'j' && key.GetString()[3] == ':'){
      return 0;
    }
  }

  return 1;
}


int is_duplicable(rapidjson::Value& key){

  if(key.IsNull()){
    return 1;
  }

  if(key.GetStringLength() >= 4){

    if(key.IsString() && std::string(key.GetString()) == "value"){
      return 0;
    }

    if(key.GetString()[0] == 'o' && key.GetString()[1] == 'b' && key.GetString()[2] == 'j' && key.GetString()[3] == ':'){
      return 0;
    }
  }
  return 1;
}

int is_key(rapidjson::Value& key){
  if (!key.IsNull() && key.IsString() && key.GetString()[0] == '/') {
    return 1;
  }
  return 0;
}

int is_key(std::string key){
    if (!key.empty() && key[0] == '/') {
        return 1;  
    } else {
        return 0;
    }
}

bool isInteger(const std::string& str) {
    if (str.empty()) return false;

    std::size_t i = 0;
    if (str[i] == '-' || str[i] == '+') ++i;

    bool hasDigits = false;
    for (; i < str.size(); ++i) {
        if (std::isdigit(str[i])) {
            hasDigits = true;
        } else {
            return false;
        }
    }

    if (!hasDigits) return false; 

    try {
        long long num = std::stoll(str);
        if (num < std::numeric_limits<int>::min() || num > std::numeric_limits<int>::max()) {
            return false;
        }
    } catch (const std::exception&) {
        return false;
    }

    return true;
}

bool isDouble(const std::string& str) {
    if (str.empty()) return false;

    std::size_t i = 0;
    if (str[i] == '-' || str[i] == '+') ++i;

    bool hasDigits = false, hasDot = false, hasExponent = false;
    
    for (; i < str.size(); ++i) {
        if (std::isdigit(str[i])) {
            hasDigits = true;
        } else if (str[i] == '.' && !hasDot) {
            hasDot = true;
        } else if ((str[i] == 'e' || str[i] == 'E') && hasDigits && !hasExponent) {
            hasExponent = true;
            hasDigits = false;
            if (i + 1 < str.size() && (str[i + 1] == '-' || str[i + 1] == '+')) ++i;
        } else {
            return false;
        }
    }
    
    return hasDigits;
}

// ----------------------------------------
// Scheduled checks
// ----------------------------------------

int should_mutate(my_mutator_t *data){
  return mutation_chance(data->mutation_rate);
}


// ----------------------------------------
// Mutation functions
// ----------------------------------------


void mutate_integer(my_mutator_t *data, rapidjson::Value& value, rapidjson::Document& document){
    value.SetInt(get_mutated_integer(value.GetInt()));
}

void mutate_double(my_mutator_t *data, rapidjson::Value& value, rapidjson::Document& document){

  double newValue = get_safe_mutated_double(data, value.GetDouble()); //get_mutated_double(data, 0.0);

  value.SetDouble(newValue);
}

void mutate_name_object(my_mutator_t *data, rapidjson::Value& key, rapidjson::Document& document){
  std::string new_key;

  if (rand() % 4 > 0) {

    // Find random valid key
    size_t pdf_keys_len = sizeof(pdf_keys) / sizeof(pdf_keys[0]);
    new_key = pdf_keys[rand() % pdf_keys_len];

  }else{

    // create mutated key
    std::string key_string(key.GetString(), key.GetStringLength());

    key_string.erase(0, 1);

    new_key = get_mutated_string(data, key_string);
    new_key = '/' + new_key;
  }

  // Replace key
  key.SetString(new_key.c_str(), new_key.length(), document.GetAllocator());
}

std::string get_new_stream_word(my_mutator_t *data){
    std::string new_string;

    int randomizer = rand() % 5; 
    if (randomizer == 0) {
        // Int
        new_string = std::to_string(get_mutated_integer(0));
    } else if(randomizer == 1){
        // Double
        new_string = std::to_string(get_mutated_double(0.0));
    }else if(randomizer == 2){
        // Name object
        new_string = get_random_key();
    } else {
        // Stream dict (slightly biased toward stream_dict value)
        new_string = get_stream_dict_value();
    }

    return new_string;
}

std::string mutate_stream_word(my_mutator_t *data, std::string initial_string){

    std::string new_string = initial_string;

    // Mutate with simmilar data or random data
    if(rand() % 2 > 0){
        if (isInteger(initial_string)) {
            // Int
            new_string = std::to_string(get_mutated_integer(std::stoi(initial_string)));
        } else if(isDouble(initial_string)){
            // Double
            new_string = std::to_string(get_mutated_double(std::stod(initial_string)));
        }else if(is_key(initial_string)){
            // Name object
            new_string = get_random_key();
        }else{
            // Stream dict
            new_string = get_stream_dict_value();
        }
    } else {

        // Get valid or randomly mutated string
        if(rand() % 5 > 0){
            new_string = get_new_stream_word(data);
        }else{
            new_string = get_mutated_string(data, initial_string);
        }
    }

    return new_string;
}

int mutate_text_stream(my_mutator_t *data, u32 init_len, u32 max_len) {
    // Ensure the buffer is treated as a string
    std::string input(reinterpret_cast<char*>(data->afl_mutate_buf), init_len);

    std::string word;
    std::vector<std::string> words;
    std::vector<std::string> separators;

    // Do random switches
    int mutation_rate = rand() % 30;

    for (size_t i = 0; i < input.size(); ++i) {
        char ch = input[i];
        
        if (ch == ' ' || ch == '\n') {
            if(!mutation_chance(mutation_rate)){
                if (!word.empty()) {
                    words.push_back(word);
                    word.clear();
                }

                // Mutate separators
                if(mutation_chance(mutation_rate)){
                    if (ch == ' '){
                        separators.emplace_back(1, '\n');
                    }else{
                        separators.emplace_back(1, ' ');
                    }
                }else{
                    separators.emplace_back(1, ch);
                }
            }else{
                // Remove item
                word.clear();
            }

            // Add new item
            if (mutation_chance(mutation_rate)) {
              words.push_back(get_new_stream_word(data));
              separators.emplace_back(1, ' ');
            }
            
        } else {
            word += ch;
        }
    }

    // Add last word
    if (!word.empty()) {
        words.push_back(word);
    }

    for (auto& w : words) {
        if (mutation_chance(mutation_rate)) {
            w = mutate_stream_word(data, w);
        }
    }

    // Combine into string again
    std::ostringstream oss;
    size_t wordIdx = 0;
    size_t separatorIdx = 0;

    while (wordIdx < words.size()) {
        oss << words[wordIdx++];
        if (separatorIdx < separators.size()) {
            oss << separators[separatorIdx++];
        }
    }

    std::string result = oss.str();

    // Result should be less than buffer size
    if (result.size() < max_len) {
        std::copy(result.begin(), result.end(), data->afl_mutate_buf);

        // Null terminate
        if (result.size() < max_len) {
            data->afl_mutate_buf[result.size()] = '\0';
        }

        return result.size();

    } else {
        return init_len;
    }
}

int mutate_stream_default(my_mutator_t *data, rapidjson::Value& value, rapidjson::Document& document){
    try {
    // Decode the Base64 string using cppcodec
    std::vector<uint8_t> decoded_data = cppcodec::base64_rfc4648::decode(std::string(value.GetString()));

    u32 init_len = decoded_data.size();

    // Check if decoded data is longer than allocated buf
    if (init_len > data->afl_mutate_buf_size){
      return 0;
    }

    // Set havoc and explore
    u32 havoc_steps = randomness(data, 16) + 1;
    bool explore = randomness(data, 2) ? true : false;

    // Place decoded data into buf
    std::memcpy(data->afl_mutate_buf, decoded_data.data(), init_len);

    u32 out_buf_len = 0;

    out_buf_len = afl_mutate(data->afl, data->afl_mutate_buf, init_len, havoc_steps, false, explore, NULL, 0, data->afl_mutate_buf_size);

    // Encode the Base64 string using cppcodec
    std::string base64_encoded = cppcodec::base64_rfc4648::encode(data->afl_mutate_buf, out_buf_len);
    value.SetString(base64_encoded.data(), base64_encoded.size(), document.GetAllocator());

    return 1;

  } catch (std::exception& e) {

  }

  return 0;
}

int mutate_stream(my_mutator_t *data, rapidjson::Value& value, rapidjson::Document& document){

  try {
    // Decode the Base64 string using cppcodec
    std::vector<uint8_t> decoded_data = cppcodec::base64_rfc4648::decode(std::string(value.GetString()));

    u32 init_len = decoded_data.size();

    // Check if decoded data is longer than allocated buf
    if (init_len > data->afl_mutate_buf_size){
      return 0;
    }

    // Place decoded data into buf
    std::memcpy(data->afl_mutate_buf, decoded_data.data(), init_len);

    u32 out_buf_len = 0;

    out_buf_len = mutate_text_stream(data, init_len, data->afl_mutate_buf_size);

    // Encode the Base64 string using cppcodec
    std::string base64_encoded = cppcodec::base64_rfc4648::encode(data->afl_mutate_buf, out_buf_len);
    value.SetString(base64_encoded.data(), base64_encoded.size(), document.GetAllocator());

    return 1;

  } catch (std::exception& e) {

  }

  return 0;
}

int duplicate(my_mutator_t* data, rapidjson::Value& value) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  value.Accept(writer);

  size_t length = buffer.GetSize();

  // Check buf size
  if (length + 1 > data->duplication_buf_size) {  
    return 0;
  }

  // Copy JSON to buf
  std::memcpy(data->duplication_buf, buffer.GetString(), length);
  data->duplication_buf[length] = '\0';

  data->has_duplicate_data = 1;

  return 1;
}

int replace(my_mutator_t* data, rapidjson::Value& value, rapidjson::Document& document) {

  const char* json = (char*)data->duplication_buf;

  // Parse JSON
  rapidjson::Document newDoc;
  if (newDoc.Parse(json).HasParseError()) {
    return 0;
  }
  
  rapidjson::Value newValue;
  newValue.CopyFrom(newDoc, document.GetAllocator());

  // Replace the value
  value.Swap(newValue);

  return 1;
}

int add_array_element(my_mutator_t* data, rapidjson::Value& array, rapidjson::Document& document){

  int random_num = randomness(data, 3);

  rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

  if(random_num == 0){
    
    // Add a new integer element to the array
    int newValue = get_interesting_num();
    array.PushBack(newValue, allocator);

  }else if(random_num == 1){
    
    // Add a new string element to the array
    size_t pdf_keys_len = sizeof(pdf_keys) / sizeof(pdf_keys[0]);

    std::string new_key = pdf_keys[randomness(data, pdf_keys_len)];
    array.PushBack(rapidjson::Value().SetString(new_key.c_str(), allocator), allocator);

  }else if(random_num == 2){

    // Add double
    double newValue = get_mutated_double(0.0);
    array.PushBack(newValue, allocator);
    
  }

  return 0;
}

int remove_array_element(my_mutator_t* data, rapidjson::Value& array, rapidjson::Document& document){

  if (!array.IsArray() || array.Empty()) {
    return 0;
  }

  // Select random index to remove
  int indexToRemove = randomness(data, array.Size());
  array.Erase(array.Begin() + indexToRemove);

  return 0;
}

int swap_value(my_mutator_t* data, rapidjson::Value& value, rapidjson::Document& document) {

  int random_num = randomness(data, 5);

  rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

  if(random_num == 0){
    
    // Add a new integer element to the array
    int newValue = get_interesting_num();

    // Replace the original value with the new value
    value.SetInt(newValue);  // INT
    
  }else if(random_num == 1){

    double newValue = get_safe_mutated_double(data, 0.0);

    value.SetDouble(newValue);  // DOUBLE

  }else if(random_num == 2){
    
    // Add a new string element to the array
    size_t pdf_keys_len = sizeof(pdf_keys) / sizeof(pdf_keys[0]);
    std::string new_key = pdf_keys[randomness(data, pdf_keys_len)];

    value.SetString(new_key.c_str(), allocator);
  }else if(random_num == 3){
    // Create a new array value
    rapidjson::Value newArray(rapidjson::kArrayType);

    newArray.PushBack(get_safe_mutated_double(data, 0.0), allocator);  // Add a double

    // Swap the old value with the new array
    value.Swap(newArray);
  }else if(random_num == 4){

    rapidjson::Value newObject(rapidjson::kObjectType);

    size_t pdf_keys_len = sizeof(pdf_keys) / sizeof(pdf_keys[0]);
    std::string new_key = pdf_keys[randomness(data, pdf_keys_len)];

    if(rand() % 2 == 0){
      newObject.AddMember(rapidjson::Value(new_key.c_str(), allocator), 
                    rapidjson::Value(get_safe_mutated_double(data, 0.0)), 
                    allocator);
    }else{
      rapidjson::Value nestedObject(rapidjson::kObjectType);

      std::string new_key2 = pdf_keys[randomness(data, pdf_keys_len)];

      nestedObject.AddMember(rapidjson::Value(new_key2.c_str(), allocator), 
                    rapidjson::Value(get_safe_mutated_double(data, 0.0)), 
                    allocator);

      newObject.AddMember(rapidjson::Value(new_key.c_str(), allocator), 
                    nestedObject, 
                    allocator);
    }

    value.Swap(newObject);

  }

  return 1;
}

void add_random_entry(my_mutator_t *data, rapidjson::Value &object, rapidjson::Document &document) {
    if (!object.IsObject()) return;

    // Generate a random key and value
    size_t pdf_keys_len = sizeof(pdf_keys) / sizeof(pdf_keys[0]);
    std::string new_key = pdf_keys[randomness(data, pdf_keys_len)];

    rapidjson::Value key(new_key.c_str(), document.GetAllocator());

    int random_value = get_interesting_num();
    rapidjson::Value value(random_value);

    swap_value(data, value, document);

    object.AddMember(key, value, document.GetAllocator());
}

void remove_random_entry(my_mutator_t *data, rapidjson::Value &object) {
    if (!object.IsObject() || object.MemberCount() == 0) return;

    // Get a random index
    std::size_t index = std::rand() % object.MemberCount();
    rapidjson::Value::MemberIterator itr = object.MemberBegin();
    std::advance(itr, index);

    // Remove
    object.RemoveMember(itr);
}