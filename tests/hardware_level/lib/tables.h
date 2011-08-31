
namespace tables
{
	//64
	const u8 kSine4bit[] = 
	{
		8,  8,  9,  10,  10,  11,  12,  12,
		13,  13,  14,  14,  14,  15,  15,  15,
		15,  15,  15,  15,  14,  14,  14,  13,
		13,  12,  12,  11,  10,  10,  9,  8,
		8,  7,  6,  5,  5,  4,  3,  3,
		2,  2,  1,  1,  1,  0,  0,  0,
		0,  0,  0,  0,  1,  1,  1,  2,
		2,  3,  3,  4,  5,  5,  6,  7
	};
	//0 -> 31 range, table size 128
	//perfect for color calculation settings
	const u8 kSine5bit[] = {
		16,  16,  17,  18,  19,  19,  20,  21,
		21,  22,  23,  23,  24,  25,  25,  26,
		26,  27,  27,  28,  28,  29,  29,  29,
		30,  30,  30,  31,  31,  31,  31,  31,
		31,  31,  31,  31,  31,  31,  30,  30,
		30,  30,  29,  29,  28,  28,  28,  27,
		27,  26,  25,  25,  24,  24,  23,  22,
		22,  21,  20,  19,  19,  18,  17,  16,
		16,  15,  14,  13,  13,  12,  11,  10,
		10,  9,  8,  8,  7,  6,  6,  5,
		5,  4,  4,  3,  3,  2,  2,  2,
		1,  1,  1,  1,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  1,  1,
		1,  1,  2,  2,  2,  3,  3,  4,
		4,  5,  5,  6,  7,  7,  8,  9,
		9,  10,  11,  11,  12,  13,  14,  14
	};

	const u8 kSine8bit[] = {
		128,  129,  131,  132,  134,  135,  137,  138,
		140,  142,  143,  145,  146,  148,  149,  151,
		152,  154,  155,  157,  158,  160,  161,  163,
		164,  166,  167,  169,  170,  172,  173,  175,
		176,  178,  179,  180,  182,  183,  185,  186,
		187,  189,  190,  192,  193,  194,  196,  197,
		198,  199,  201,  202,  203,  205,  206,  207,
		208,  209,  211,  212,  213,  214,  215,  216,
		217,  219,  220,  221,  222,  223,  224,  225,
		226,  227,  228,  229,  230,  231,  232,  232,
		233,  234,  235,  236,  237,  237,  238,  239,
		240,  240,  241,  242,  243,  243,  244,  245,
		245,  246,  246,  247,  247,  248,  248,  249,
		249,  250,  250,  251,  251,  251,  252,  252,
		252,  253,  253,  253,  254,  254,  254,  254,
		254,  254,  255,  255,  255,  255,  255,  255,
		255,  255,  255,  255,  255,  255,  255,  255,
		254,  254,  254,  254,  254,  253,  253,  253,
		253,  252,  252,  252,  251,  251,  251,  250,
		250,  249,  249,  248,  248,  247,  247,  246,
		246,  245,  244,  244,  243,  242,  242,  241,
		240,  240,  239,  238,  237,  236,  236,  235,
		234,  233,  232,  231,  230,  229,  228,  228,
		227,  226,  225,  224,  223,  221,  220,  219,
		218,  217,  216,  215,  214,  213,  211,  210,
		209,  208,  207,  205,  204,  203,  202,  200,
		199,  198,  196,  195,  194,  192,  191,  190,
		188,  187,  186,  184,  183,  181,  180,  179,
		177,  176,  174,  173,  171,  170,  168,  167,
		165,  164,  162,  161,  159,  158,  156,  155,
		153,  152,  150,  149,  147,  146,  144,  143,
		141,  140,  138,  136,  135,  133,  132,  130,
		129,  127,  125,  124,  122,  121,  119,  118,
		116,  115,  113,  111,  110,  108,  107,  105,
		104,  102,  101,  99,  98,  96,  95,  93,
		92,  90,  89,  87,  86,  84,  83,  81,
		80,  78,  77,  76,  74,  73,  71,  70,
		69,  67,  66,  64,  63,  62,  60,  59,
		58,  56,  55,  54,  53,  51,  50,  49,
		48,  46,  45,  44,  43,  42,  41,  39,
		38,  37,  36,  35,  34,  33,  32,  31,
		30,  29,  28,  27,  26,  25,  24,  23,
		22,  21,  21,  20,  19,  18,  17,  17,
		16,  15,  14,  14,  13,  12,  12,  11,
		10,  10,  9,  9,  8,  7,  7,  6,
		6,  5,  5,  5,  4,  4,  3,  3,
		3,  2,  2,  2,  2,  1,  1,  1,
		1,  1,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  1,  1,  1,  1,  1,  2,  2,
		2,  2,  3,  3,  3,  4,  4,  5,
		5,  5,  6,  6,  7,  7,  8,  8,
		9,  10,  10,  11,  11,  12,  13,  13,
		14,  15,  16,  16,  17,  18,  19,  20,
		20,  21,  22,  23,  24,  25,  26,  27,
		28,  29,  30,  31,  32,  33,  34,  35,
		36,  37,  38,  39,  40,  42,  43,  44,
		45,  46,  47,  49,  50,  51,  52,  54,
		55,  56,  58,  59,  60,  62,  63,  64,
		66,  67,  68,  70,  71,  73,  74,  75,
		77,  78,  80,  81,  83,  84,  86,  87,
		88,  90,  91,  93,  94,  96,  97,  99,
		101,  102,  104,  105,  107,  108,  110,  111,
		113,  114,  116,  117,  119,  121,  122,  124
	};

	//512 size
	u16 kSine9bit[] = 
	{
		256,  259,  262,  265,  268,  271,  274,  277,
		280,  284,  287,  290,  293,  296,  299,  302,
		305,  308,  311,  314,  317,  320,  323,  326,
		329,  332,  335,  338,  341,  344,  347,  350,
		353,  356,  359,  362,  364,  367,  370,  373,
		376,  378,  381,  384,  387,  389,  392,  394,
		397,  400,  402,  405,  407,  410,  412,  415,
		417,  420,  422,  424,  427,  429,  431,  434,
		436,  438,  440,  442,  444,  447,  449,  451,
		453,  455,  457,  458,  460,  462,  464,  466,
		468,  469,  471,  473,  474,  476,  477,  479,
		480,  482,  483,  485,  486,  487,  489,  490,
		491,  492,  494,  495,  496,  497,  498,  499,
		500,  501,  501,  502,  503,  504,  505,  505,
		506,  507,  507,  508,  508,  509,  509,  509,
		510,  510,  510,  510,  511,  511,  511,  511,
		511,  511,  511,  511,  511,  511,  510,  510,
		510,  510,  509,  509,  508,  508,  507,  507,
		506,  506,  505,  504,  504,  503,  502,  501,
		500,  499,  499,  498,  497,  495,  494,  493,
		492,  491,  490,  488,  487,  486,  484,  483,
		482,  480,  479,  477,  475,  474,  472,  470,
		469,  467,  465,  463,  462,  460,  458,  456,
		454,  452,  450,  448,  446,  444,  442,  440,
		437,  435,  433,  431,  428,  426,  424,  421,
		419,  417,  414,  412,  409,  407,  404,  402,
		399,  396,  394,  391,  388,  386,  383,  380,
		378,  375,  372,  369,  366,  364,  361,  358,
		355,  352,  349,  346,  343,  341,  338,  335,
		332,  329,  326,  323,  320,  317,  314,  310,
		307,  304,  301,  298,  295,  292,  289,  286,
		283,  280,  276,  273,  270,  267,  264,  261,
		258,  255,  251,  248,  245,  242,  239,  236,
		233,  230,  227,  223,  220,  217,  214,  211,
		208,  205,  202,  199,  196,  193,  190,  187,
		184,  181,  178,  175,  172,  169,  166,  163,
		160,  157,  154,  151,  149,  146,  143,  140,
		137,  135,  132,  129,  126,  124,  121,  118,
		116,  113,  111,  108,  105,  103,  100,  98,
		96,  93,  91,  88,  86,  84,  81,  79,
		77,  75,  72,  70,  68,  66,  64,  62,
		60,  58,  56,  54,  52,  50,  48,  46,
		45,  43,  41,  40,  38,  36,  35,  33,
		32,  30,  29,  27,  26,  24,  23,  22,
		21,  19,  18,  17,  16,  15,  14,  13,
		12,  11,  10,  9,  8,  8,  7,  6,
		6,  5,  4,  4,  3,  3,  2,  2,
		2,  1,  1,  1,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  1,
		1,  1,  2,  2,  2,  3,  3,  4,
		4,  5,  5,  6,  7,  8,  8,  9,
		10,  11,  12,  13,  14,  15,  16,  17,
		18,  19,  20,  22,  23,  24,  26,  27,
		28,  30,  31,  33,  34,  36,  38,  39,
		41,  43,  44,  46,  48,  50,  52,  54,
		56,  58,  60,  62,  64,  66,  68,  70,
		72,  74,  77,  79,  81,  83,  86,  88,
		90,  93,  95,  98,  100,  103,  105,  108,
		110,  113,  115,  118,  121,  123,  126,  129,
		131,  134,  137,  140,  143,  145,  148,  151,
		154,  157,  160,  163,  165,  168,  171,  174,
		177,  180,  183,  186,  189,  192,  195,  198,
		201,  204,  208,  211,  214,  217,  220,  223,
		226,  229,  232,  235,  239,  242,  245,  248

	};


	//sine table from 0 -> 2047 for testing the full 11 bit range of line / cell scrolling
	//size 512
#if 0
	u16 kSine11bit[] = {
		1024,  1036,  1049,  1061,  1074,  1086,  1099,  1111,
		1124,  1136,  1148,  1161,  1173,  1186,  1198,  1210,
		1223,  1235,  1247,  1259,  1272,  1284,  1296,  1308,
		1320,  1332,  1344,  1356,  1367,  1379,  1391,  1403,
		1414,  1426,  1437,  1449,  1460,  1471,  1482,  1494,
		1505,  1516,  1527,  1538,  1548,  1559,  1570,  1580,
		1591,  1601,  1611,  1622,  1632,  1642,  1652,  1662,
		1671,  1681,  1690,  1700,  1709,  1719,  1728,  1737,
		1746,  1754,  1763,  1772,  1780,  1789,  1797,  1805,
		1813,  1821,  1829,  1836,  1844,  1851,  1859,  1866,
		1873,  1880,  1887,  1893,  1900,  1906,  1913,  1919,
		1925,  1931,  1936,  1942,  1947,  1953,  1958,  1963,
		1968,  1973,  1977,  1982,  1986,  1990,  1994,  1998,
		2002,  2005,  2009,  2012,  2015,  2018,  2021,  2024,
		2027,  2029,  2031,  2033,  2035,  2037,  2039,  2040,
		2042,  2043,  2044,  2045,  2046,  2046,  2047,  2047,
		2047,  2047,  2047,  2046,  2046,  2045,  2045,  2044,
		2043,  2041,  2040,  2038,  2037,  2035,  2033,  2031,
		2028,  2026,  2023,  2020,  2018,  2015,  2011,  2008,
		2004,  2001,  1997,  1993,  1989,  1985,  1980,  1976,
		1971,  1966,  1961,  1956,  1951,  1946,  1940,  1935,
		1929,  1923,  1917,  1911,  1904,  1898,  1891,  1885,
		1878,  1871,  1864,  1857,  1849,  1842,  1834,  1827,
		1819,  1811,  1803,  1795,  1786,  1778,  1769,  1761,
		1752,  1743,  1734,  1725,  1716,  1707,  1697,  1688,
		1678,  1669,  1659,  1649,  1639,  1629,  1619,  1608,
		1598,  1588,  1577,  1567,  1556,  1545,  1534,  1524,
		1513,  1502,  1490,  1479,  1468,  1457,  1445,  1434,
		1422,  1411,  1399,  1388,  1376,  1364,  1352,  1340,
		1328,  1316,  1304,  1292,  1280,  1268,  1256,  1244,
		1231,  1219,  1207,  1194,  1182,  1170,  1157,  1145,
		1132,  1120,  1107,  1095,  1083,  1070,  1057,  1045,
		1032,  1020,  1007,  995,  982,  970,  957,  945,
		932,  920,  907,  895,  883,  870,  858,  845,
		833,  821,  809,  796,  784,  772,  760,  748,
		736,  724,  712,  700,  688,  676,  664,  653,
		641,  630,  618,  607,  595,  584,  573,  561,
		550,  539,  528,  517,  506,  496,  485,  474,
		464,  453,  443,  433,  422,  412,  402,  392,
		383,  373,  363,  354,  344,  335,  326,  317,
		308,  299,  290,  281,  273,  264,  256,  248,
		240,  232,  224,  216,  208,  201,  193,  186,
		179,  172,  165,  158,  152,  145,  139,  133,
		127,  121,  115,  109,  104,  98,  93,  88,
		83,  78,  73,  69,  64,  60,  56,  52,
		48,  44,  41,  37,  34,  31,  28,  25,
		22,  20,  17,  15,  13,  11,  9,  8,
		6,  5,  4,  3,  2,  1,  1,  0,
		0,  0,  0,  0,  1,  1,  2,  3,
		4,  5,  6,  8,  9,  11,  13,  15,
		17,  19,  22,  25,  27,  30,  33,  37,
		40,  44,  47,  51,  55,  59,  63,  68,
		72,  77,  82,  87,  92,  97,  103,  108,
		114,  120,  126,  132,  138,  144,  151,  157,
		164,  171,  178,  185,  192,  200,  207,  215,
		223,  231,  239,  247,  255,  263,  272,  280,
		289,  298,  306,  315,  325,  334,  343,  352,
		362,  372,  381,  391,  401,  411,  421,  431,
		441,  452,  462,  473,  483,  494,  505,  516,
		527,  538,  549,  560,  571,  582,  594,  605,
		616,  628,  640,  651,  663,  675,  686,  698,
		710,  722,  734,  746,  758,  770,  782,  795,
		807,  819,  831,  844,  856,  868,  881,  893,
		906,  918,  931,  943,  956,  968,  981,  993,
	};
#endif
	u16 kSine11bit[] = {
		1024,  1027,  1030,  1033,  1037,  1040,  1043,  1046,
		1049,  1052,  1055,  1058,  1062,  1065,  1068,  1071,
		1074,  1077,  1080,  1083,  1087,  1090,  1093,  1096,
		1099,  1102,  1105,  1108,  1112,  1115,  1118,  1121,
		1124,  1127,  1130,  1133,  1137,  1140,  1143,  1146,
		1149,  1152,  1155,  1158,  1161,  1165,  1168,  1171,
		1174,  1177,  1180,  1183,  1186,  1189,  1192,  1195,
		1199,  1202,  1205,  1208,  1211,  1214,  1217,  1220,
		1223,  1226,  1229,  1232,  1236,  1239,  1242,  1245,
		1248,  1251,  1254,  1257,  1260,  1263,  1266,  1269,
		1272,  1275,  1278,  1281,  1284,  1287,  1290,  1293,
		1296,  1299,  1302,  1305,  1308,  1311,  1314,  1317,
		1320,  1323,  1326,  1329,  1332,  1335,  1338,  1341,
		1344,  1347,  1350,  1353,  1356,  1359,  1362,  1365,
		1368,  1371,  1374,  1377,  1380,  1383,  1386,  1389,
		1392,  1394,  1397,  1400,  1403,  1406,  1409,  1412,
		1415,  1418,  1421,  1424,  1426,  1429,  1432,  1435,
		1438,  1441,  1444,  1446,  1449,  1452,  1455,  1458,
		1461,  1464,  1466,  1469,  1472,  1475,  1478,  1480,
		1483,  1486,  1489,  1492,  1494,  1497,  1500,  1503,
		1505,  1508,  1511,  1514,  1517,  1519,  1522,  1525,
		1527,  1530,  1533,  1536,  1538,  1541,  1544,  1546,
		1549,  1552,  1554,  1557,  1560,  1563,  1565,  1568,
		1570,  1573,  1576,  1578,  1581,  1584,  1586,  1589,
		1592,  1594,  1597,  1599,  1602,  1604,  1607,  1610,
		1612,  1615,  1617,  1620,  1622,  1625,  1627,  1630,
		1633,  1635,  1638,  1640,  1643,  1645,  1648,  1650,
		1653,  1655,  1657,  1660,  1662,  1665,  1667,  1670,
		1672,  1675,  1677,  1679,  1682,  1684,  1687,  1689,
		1691,  1694,  1696,  1698,  1701,  1703,  1705,  1708,
		1710,  1712,  1715,  1717,  1719,  1722,  1724,  1726,
		1729,  1731,  1733,  1735,  1738,  1740,  1742,  1744,
		1746,  1749,  1751,  1753,  1755,  1758,  1760,  1762,
		1764,  1766,  1768,  1771,  1773,  1775,  1777,  1779,
		1781,  1783,  1785,  1787,  1790,  1792,  1794,  1796,
		1798,  1800,  1802,  1804,  1806,  1808,  1810,  1812,
		1814,  1816,  1818,  1820,  1822,  1824,  1826,  1828,
		1830,  1832,  1834,  1835,  1837,  1839,  1841,  1843,
		1845,  1847,  1849,  1850,  1852,  1854,  1856,  1858,
		1860,  1861,  1863,  1865,  1867,  1869,  1870,  1872,
		1874,  1876,  1877,  1879,  1881,  1883,  1884,  1886,
		1888,  1889,  1891,  1893,  1894,  1896,  1898,  1899,
		1901,  1902,  1904,  1906,  1907,  1909,  1910,  1912,
		1913,  1915,  1917,  1918,  1920,  1921,  1923,  1924,
		1926,  1927,  1929,  1930,  1932,  1933,  1934,  1936,
		1937,  1939,  1940,  1941,  1943,  1944,  1946,  1947,
		1948,  1950,  1951,  1952,  1954,  1955,  1956,  1958,
		1959,  1960,  1961,  1963,  1964,  1965,  1966,  1968,
		1969,  1970,  1971,  1972,  1974,  1975,  1976,  1977,
		1978,  1979,  1980,  1982,  1983,  1984,  1985,  1986,
		1987,  1988,  1989,  1990,  1991,  1992,  1993,  1994,
		1995,  1996,  1997,  1998,  1999,  2000,  2001,  2002,
		2003,  2004,  2005,  2006,  2006,  2007,  2008,  2009,
		2010,  2011,  2012,  2012,  2013,  2014,  2015,  2016,
		2016,  2017,  2018,  2019,  2019,  2020,  2021,  2022,
		2022,  2023,  2024,  2024,  2025,  2026,  2026,  2027,
		2028,  2028,  2029,  2029,  2030,  2031,  2031,  2032,
		2032,  2033,  2033,  2034,  2034,  2035,  2035,  2036,
		2036,  2037,  2037,  2038,  2038,  2039,  2039,  2039,
		2040,  2040,  2041,  2041,  2041,  2042,  2042,  2042,
		2043,  2043,  2043,  2044,  2044,  2044,  2044,  2045,
		2045,  2045,  2045,  2046,  2046,  2046,  2046,  2046,
		2047,  2047,  2047,  2047,  2047,  2047,  2047,  2047,
		2048,  2048,  2048,  2048,  2048,  2048,  2048,  2048,
		2048,  2048,  2048,  2048,  2048,  2048,  2048,  2048,
		2048,  2048,  2048,  2048,  2047,  2047,  2047,  2047,
		2047,  2047,  2047,  2047,  2046,  2046,  2046,  2046,
		2046,  2045,  2045,  2045,  2045,  2044,  2044,  2044,
		2044,  2043,  2043,  2043,  2042,  2042,  2042,  2041,
		2041,  2041,  2040,  2040,  2039,  2039,  2038,  2038,
		2038,  2037,  2037,  2036,  2036,  2035,  2035,  2034,
		2034,  2033,  2033,  2032,  2032,  2031,  2030,  2030,
		2029,  2029,  2028,  2027,  2027,  2026,  2026,  2025,
		2024,  2024,  2023,  2022,  2021,  2021,  2020,  2019,
		2019,  2018,  2017,  2016,  2015,  2015,  2014,  2013,
		2012,  2011,  2011,  2010,  2009,  2008,  2007,  2006,
		2005,  2005,  2004,  2003,  2002,  2001,  2000,  1999,
		1998,  1997,  1996,  1995,  1994,  1993,  1992,  1991,
		1990,  1989,  1988,  1987,  1986,  1985,  1984,  1982,
		1981,  1980,  1979,  1978,  1977,  1976,  1975,  1973,
		1972,  1971,  1970,  1969,  1967,  1966,  1965,  1964,
		1962,  1961,  1960,  1959,  1957,  1956,  1955,  1953,
		1952,  1951,  1949,  1948,  1947,  1945,  1944,  1943,
		1941,  1940,  1938,  1937,  1936,  1934,  1933,  1931,
		1930,  1928,  1927,  1925,  1924,  1922,  1921,  1919,
		1918,  1916,  1915,  1913,  1912,  1910,  1909,  1907,
		1905,  1904,  1902,  1901,  1899,  1897,  1896,  1894,
		1892,  1891,  1889,  1887,  1886,  1884,  1882,  1881,
		1879,  1877,  1875,  1874,  1872,  1870,  1868,  1867,
		1865,  1863,  1861,  1859,  1858,  1856,  1854,  1852,
		1850,  1848,  1846,  1845,  1843,  1841,  1839,  1837,
		1835,  1833,  1831,  1829,  1827,  1826,  1824,  1822,
		1820,  1818,  1816,  1814,  1812,  1810,  1808,  1806,
		1804,  1802,  1800,  1797,  1795,  1793,  1791,  1789,
		1787,  1785,  1783,  1781,  1779,  1777,  1774,  1772,
		1770,  1768,  1766,  1764,  1762,  1759,  1757,  1755,
		1753,  1751,  1748,  1746,  1744,  1742,  1739,  1737,
		1735,  1733,  1730,  1728,  1726,  1724,  1721,  1719,
		1717,  1714,  1712,  1710,  1707,  1705,  1703,  1700,
		1698,  1696,  1693,  1691,  1689,  1686,  1684,  1681,
		1679,  1677,  1674,  1672,  1669,  1667,  1664,  1662,
		1660,  1657,  1655,  1652,  1650,  1647,  1645,  1642,
		1640,  1637,  1635,  1632,  1630,  1627,  1625,  1622,
		1619,  1617,  1614,  1612,  1609,  1607,  1604,  1602,
		1599,  1596,  1594,  1591,  1589,  1586,  1583,  1581,
		1578,  1575,  1573,  1570,  1567,  1565,  1562,  1559,
		1557,  1554,  1551,  1549,  1546,  1543,  1541,  1538,
		1535,  1532,  1530,  1527,  1524,  1522,  1519,  1516,
		1513,  1511,  1508,  1505,  1502,  1500,  1497,  1494,
		1491,  1488,  1486,  1483,  1480,  1477,  1474,  1472,
		1469,  1466,  1463,  1460,  1457,  1455,  1452,  1449,
		1446,  1443,  1440,  1437,  1435,  1432,  1429,  1426,
		1423,  1420,  1417,  1414,  1412,  1409,  1406,  1403,
		1400,  1397,  1394,  1391,  1388,  1385,  1382,  1379,
		1376,  1374,  1371,  1368,  1365,  1362,  1359,  1356,
		1353,  1350,  1347,  1344,  1341,  1338,  1335,  1332,
		1329,  1326,  1323,  1320,  1317,  1314,  1311,  1308,
		1305,  1302,  1299,  1296,  1293,  1290,  1287,  1284,
		1281,  1278,  1275,  1272,  1269,  1266,  1263,  1260,
		1256,  1253,  1250,  1247,  1244,  1241,  1238,  1235,
		1232,  1229,  1226,  1223,  1220,  1217,  1214,  1210,
		1207,  1204,  1201,  1198,  1195,  1192,  1189,  1186,
		1183,  1180,  1176,  1173,  1170,  1167,  1164,  1161,
		1158,  1155,  1152,  1149,  1145,  1142,  1139,  1136,
		1133,  1130,  1127,  1124,  1121,  1117,  1114,  1111,
		1108,  1105,  1102,  1099,  1096,  1092,  1089,  1086,
		1083,  1080,  1077,  1074,  1071,  1067,  1064,  1061,
		1058,  1055,  1052,  1049,  1045,  1042,  1039,  1036,
		1033,  1030,  1027,  1024,  1020,  1017,  1014,  1011,
		1008,  1005,  1002,  998,  995,  992,  989,  986,
		983,  980,  977,  973,  970,  967,  964,  961,
		958,  955,  952,  948,  945,  942,  939,  936,
		933,  930,  927,  923,  920,  917,  914,  911,
		908,  905,  902,  899,  895,  892,  889,  886,
		883,  880,  877,  874,  871,  868,  864,  861,
		858,  855,  852,  849,  846,  843,  840,  837,
		834,  830,  827,  824,  821,  818,  815,  812,
		809,  806,  803,  800,  797,  794,  791,  788,
		785,  781,  778,  775,  772,  769,  766,  763,
		760,  757,  754,  751,  748,  745,  742,  739,
		736,  733,  730,  727,  724,  721,  718,  715,
		712,  709,  706,  703,  700,  697,  694,  691,
		688,  685,  682,  680,  677,  674,  671,  668,
		665,  662,  659,  656,  653,  650,  647,  644,
		641,  639,  636,  633,  630,  627,  624,  621,
		618,  615,  613,  610,  607,  604,  601,  598,
		595,  593,  590,  587,  584,  581,  578,  576,
		573,  570,  567,  564,  562,  559,  556,  553,
		550,  548,  545,  542,  539,  537,  534,  531,
		528,  526,  523,  520,  517,  515,  512,  509,
		507,  504,  501,  498,  496,  493,  490,  488,
		485,  482,  480,  477,  474,  472,  469,  467,
		464,  461,  459,  456,  454,  451,  448,  446,
		443,  441,  438,  435,  433,  430,  428,  425,
		423,  420,  418,  415,  413,  410,  408,  405,
		403,  400,  398,  395,  393,  390,  388,  385,
		383,  380,  378,  376,  373,  371,  368,  366,
		363,  361,  359,  356,  354,  352,  349,  347,
		345,  342,  340,  338,  335,  333,  331,  328,
		326,  324,  321,  319,  317,  315,  312,  310,
		308,  306,  303,  301,  299,  297,  295,  292,
		290,  288,  286,  284,  281,  279,  277,  275,
		273,  271,  269,  267,  264,  262,  260,  258,
		256,  254,  252,  250,  248,  246,  244,  242,
		240,  238,  236,  234,  232,  230,  228,  226,
		224,  222,  220,  218,  216,  214,  212,  210,
		208,  207,  205,  203,  201,  199,  197,  195,
		194,  192,  190,  188,  186,  184,  183,  181,
		179,  177,  176,  174,  172,  170,  169,  167,
		165,  164,  162,  160,  158,  157,  155,  153,
		152,  150,  149,  147,  145,  144,  142,  141,
		139,  137,  136,  134,  133,  131,  130,  128,
		127,  125,  124,  122,  121,  119,  118,  116,
		115,  113,  112,  111,  109,  108,  106,  105,
		104,  102,  101,  99,  98,  97,  95,  94,
		93,  92,  90,  89,  88,  86,  85,  84,
		83,  81,  80,  79,  78,  77,  75,  74,
		73,  72,  71,  70,  69,  67,  66,  65,
		64,  63,  62,  61,  60,  59,  58,  57,
		56,  55,  54,  53,  52,  51,  50,  49,
		48,  47,  46,  45,  44,  43,  42,  41,
		41,  40,  39,  38,  37,  36,  35,  35,
		34,  33,  32,  31,  31,  30,  29,  28,
		28,  27,  26,  26,  25,  24,  24,  23,
		22,  22,  21,  20,  20,  19,  19,  18,
		17,  17,  16,  16,  15,  15,  14,  14,
		13,  13,  12,  12,  11,  11,  10,  10,
		9,  9,  9,  8,  8,  7,  7,  7,
		6,  6,  6,  5,  5,  5,  4,  4,
		4,  4,  3,  3,  3,  3,  2,  2,
		2,  2,  2,  1,  1,  1,  1,  1,
		1,  1,  1,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  1,
		1,  1,  1,  1,  1,  1,  2,  2,
		2,  2,  2,  2,  3,  3,  3,  3,
		4,  4,  4,  5,  5,  5,  5,  6,
		6,  6,  7,  7,  8,  8,  8,  9,
		9,  10,  10,  10,  11,  11,  12,  12,
		13,  13,  14,  14,  15,  15,  16,  16,
		17,  18,  18,  19,  19,  20,  21,  21,
		22,  23,  23,  24,  25,  25,  26,  27,
		27,  28,  29,  30,  30,  31,  32,  33,
		33,  34,  35,  36,  37,  37,  38,  39,
		40,  41,  42,  43,  44,  45,  45,  46,
		47,  48,  49,  50,  51,  52,  53,  54,
		55,  56,  57,  58,  59,  60,  61,  62,
		64,  65,  66,  67,  68,  69,  70,  71,
		72,  74,  75,  76,  77,  78,  80,  81,
		82,  83,  85,  86,  87,  88,  90,  91,
		92,  93,  95,  96,  97,  99,  100,  101,
		103,  104,  106,  107,  108,  110,  111,  113,
		114,  115,  117,  118,  120,  121,  123,  124,
		126,  127,  129,  130,  132,  133,  135,  137,
		138,  140,  141,  143,  144,  146,  148,  149,
		151,  153,  154,  156,  158,  159,  161,  163,
		164,  166,  168,  169,  171,  173,  175,  176,
		178,  180,  182,  183,  185,  187,  189,  191,
		193,  194,  196,  198,  200,  202,  204,  206,
		207,  209,  211,  213,  215,  217,  219,  221,
		223,  225,  227,  229,  231,  233,  235,  237,
		239,  241,  243,  245,  247,  249,  251,  253,
		255,  257,  259,  261,  263,  265,  267,  270,
		272,  274,  276,  278,  280,  282,  285,  287,
		289,  291,  293,  296,  298,  300,  302,  304,
		307,  309,  311,  313,  316,  318,  320,  322,
		325,  327,  329,  332,  334,  336,  339,  341,
		343,  346,  348,  350,  353,  355,  357,  360,
		362,  365,  367,  369,  372,  374,  377,  379,
		381,  384,  386,  389,  391,  394,  396,  399,
		401,  404,  406,  409,  411,  414,  416,  419,
		421,  424,  426,  429,  431,  434,  437,  439,
		442,  444,  447,  449,  452,  455,  457,  460,
		462,  465,  468,  470,  473,  476,  478,  481,
		484,  486,  489,  492,  494,  497,  500,  502,
		505,  508,  510,  513,  516,  519,  521,  524,
		527,  530,  532,  535,  538,  541,  543,  546,
		549,  552,  554,  557,  560,  563,  566,  568,
		571,  574,  577,  580,  582,  585,  588,  591,
		594,  597,  600,  602,  605,  608,  611,  614,
		617,  620,  622,  625,  628,  631,  634,  637,
		640,  643,  646,  649,  651,  654,  657,  660,
		663,  666,  669,  672,  675,  678,  681,  684,
		687,  690,  693,  696,  699,  702,  705,  708,
		710,  713,  716,  719,  722,  725,  728,  731,
		734,  737,  740,  743,  746,  749,  753,  756,
		759,  762,  765,  768,  771,  774,  777,  780,
		783,  786,  789,  792,  795,  798,  801,  804,
		807,  810,  813,  816,  820,  823,  826,  829,
		832,  835,  838,  841,  844,  847,  850,  853,
		857,  860,  863,  866,  869,  872,  875,  878,
		881,  884,  887,  891,  894,  897,  900,  903,
		906,  909,  912,  915,  919,  922,  925,  928,
		931,  934,  937,  940,  944,  947,  950,  953,
		956,  959,  962,  965,  969,  972,  975,  978,
		981,  984,  987,  990,  994,  997,  1000,  1003
	};
}