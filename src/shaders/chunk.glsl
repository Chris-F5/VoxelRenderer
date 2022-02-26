#define NEIGHBOUR_EMPTY ~0
#define NEIGHBOUR_NEGATIVE_X 0
#define NEIGHBOUR_NEGATIVE_Y 1
#define NEIGHBOUR_NEGATIVE_Z 2
#define NEIGHBOUR_POSITIVE_X 3
#define NEIGHBOUR_POSITIVE_Y 4
#define NEIGHBOUR_POSITIVE_Z 5

const vec3 normalPalette[] = {
	vec3(0.0, 1.0, 0.0),
	vec3(-0.09217040102111906, 0.9921568627450981, 0.08443563752001125),
	vec3(0.015424259197516546, 0.9843137254901961, -0.17575147805870264),
	vec3(0.13121021817356954, 0.9764705882352941, 0.17114049479378932),
	vec3(-0.24471836201897004, 0.9686274509803922, -0.04328723250544358),
	vec3(0.23397062377407862, 0.9607843137254902, -0.14883295908436192),
	vec3(-0.07870041162902662, 0.9529411764705882, 0.29276160847398186),
	vec3(-0.1506183906707799, 0.9450980392156862, -0.29000654589579433),
	vec3(0.32748955578915345, 0.9372549019607843, 0.11959866052554075),
	vec3(-0.3411245542319837, 0.9294117647058824, 0.1408112571008724),
	vec3(0.16454357968156566, 0.9215686274509804, -0.3516200723560763),
	vec3(0.12160862385489125, 0.9137254901960784, 0.3877074556544609),
	vec3(-0.366442394719015, 0.9058823529411765, -0.21236085793373355),
	vec3(0.42965472243150893, 0.8980392156862745, -0.0944583854503343),
	vec3(-0.26201608017146794, 0.8901960784313725, 0.3726909117177189),
	vec3(-0.060475618728938, 0.8823529411764706, -0.46668617585759276),
	vec3(0.37086075605480223, 0.8745098039215686, 0.3125618378233657),
	vec3(-0.4984616284589845, 0.8666666666666667, 0.02061295327958017),
	vec3(0.36311314578754866, 0.8588235294117648, -0.3613460788288565),
	vec3(-0.024259630089262248, 0.8509803921568627, 0.5246368672829659),
	vec3(-0.34450828213249896, 0.8431372549019608, -0.41283606060820394),
	vec3(0.5448932952382085, 0.8352941176470589, 0.07331462220231286),
	vec3(-0.4609431119886608, 0.8274509803921568, 0.32071221142679845),
	vec3(0.12574634965417986, 0.8196078431372549, -0.5589551314878017),
	vec3(0.29034599535752936, 0.8117647058823529, 0.5066924760281946),
	vec3(-0.5666000522298087, 0.803921568627451, -0.18076087051338485),
	vec3(0.5493892623987536, 0.7960784313725491, -0.25383177394560946),
	vec3(-0.23757194023950373, 0.788235294117647, 0.5676660059560588),
	vec3(-0.2116309604491467, 0.7803921568627451, -0.5883879826156244),
	vec3(0.5620555709479339, 0.7725490196078432, 0.29540065583771563),
	vec3(-0.6230956285729644, 0.7647058823529411, 0.16424600801259084),
	vec3(0.35347740606107797, 0.7568627450980392, -0.5497385819523607),
	vec3(0.1122198155498277, 0.7490196078431373, 0.6529749919135308),
	vec3(-0.5307534339784455, 0.7411764705882353, -0.4110452916242377),
	vec3(0.6775479734831265, 0.7333333333333334, -0.05613346462788963),
	vec3(-0.4673644769675879, 0.7254901960784313, 0.5052073050371423),
	vec3(0.0033972663271322765, 0.7176470588235294, -0.6963987058743291),
	vec3(0.4732754503714691, 0.7098039215686274, 0.5217171082123729),
	vec3(-0.7091763603137669, 0.7019607843137254, -0.06572630567571647),
	vec3(0.5734125370153066, 0.6941176470588235, -0.4351996719153086),
	vec3(-0.1301823895537059, 0.6862745098039216, 0.7155975423682465),
	vec3(-0.3912860400493058, 0.6784313725490196, -0.6217926564408639),
	vec3(0.7154482050190577, 0.6705882352941177, 0.19607469143193745),
	vec3(-0.6662371986914292, 0.6627450980392157, 0.3419019305367953),
	vec3(0.2626988441055051, 0.6549019607843137, -0.7085850259965227),
	vec3(0.2874724325296579, 0.6470588235294117, 0.706168874581863),
	vec3(-0.6949362109173589, 0.6392156862745098, -0.3293432391539737),
	vec3(0.7410582562692147, 0.6313725490196078, -0.22847618072725584),
	vec3(-0.3952955031350482, 0.6235294117647059, 0.6745016959693793),
	vec3(-0.16544066438731284, 0.615686274509804, -0.7704283210963385),
	vec3(0.6473527344125956, 0.607843137254902, 0.45984905973662027),
	vec3(-0.7938585874078181, 0.6, 0.09893706686001819),
	vec3(0.5223096944843154, 0.592156862745098, -0.6136308604947963),
	vec3(0.02957469098140391, 0.584313725490196, 0.8109887840513716),
	vec3(-0.5735462328143757, 0.5764705882352941, -0.5820020444329145),
	vec3(0.8215220047477721, 0.5686274509803921, 0.04200497240498129),
	vec3(-0.6382759402297882, 0.5607843137254902, 0.5273753668908048),
	vec3(0.11513447360432466, 0.5529411764705883, -0.8252273070804047),
	vec3(0.47545827383542627, 0.5450980392156863, 0.6905125324602377),
	vec3(-0.8219417238285813, 0.5372549019607843, -0.18912686998099534),
	vec3(0.7381301385470803, 0.5294117647058824, -0.4181950286163999),
	vec3(-0.26328236628919355, 0.5215686274509803, 0.8115716619375417),
	vec3(-0.35604179751436466, 0.5137254901960784, -0.7805897508586307),
	vec3(0.7940936716979322, 0.5058823529411764, 0.3368950660843753),
	vec3(-0.8174001511927452, 0.4980392156862745, 0.2895063599796692),
	vec3(0.40925977033506833, 0.4901960784313726, -0.7695545757617134),
	vec3(0.21914316522863242, 0.48235294117647054, 0.8481225815128287),
	vec3(-0.7380709544794888, 0.4745098039215686, -0.47967875931299153),
	vec3(0.8723749257487726, 0.4666666666666667, -0.1455479685431774),
	vec3(-0.5474684943224817, 0.45882352941176474, 0.6998279907108647),
	vec3(-0.06935209635282837, 0.4509803921568627, -0.889835362649468),
	vec3(0.6550776837241423, 0.44313725490196076, 0.6119661768468642),
	vec3(-0.9002454550764422, 0.43529411764705883, -0.00878360723609864),
	vec3(0.6725361042416287, 0.4274509803921569, -0.6041364480423914),
	vec3(-0.08817509675818322, 0.41960784313725485, 0.9034126467397864),
	vec3(-0.5473821190816276, 0.4117647058823529, -0.7285757631841223),
	vec3(0.8992121431681919, 0.403921568627451, 0.1681216463647876),
	vec3(-0.7795216046843841, 0.39607843137254906, 0.48525039312889134),
	vec3(0.24791251535927467, 0.388235294117647, -0.8875881596380281),
	vec3(0.41823073459029964, 0.3803921568627451, 0.8248544475490006),
	vec3(-0.8685545248201498, 0.37254901960784315, -0.3268336968607716),
	vec3(0.8641044607459363, 0.3647058823529412, -0.3468617884635568),
	vec3(-0.40417468742048124, 0.35686274509803917, 0.8421946349922018),
	vec3(-0.27172634062193296, 0.34901960784313724, -0.8968556791107676),
	vec3(0.8086607597605099, 0.3411764705882353, 0.47923521525500384),
	vec3(-0.9227500112289794, 0.33333333333333337, 0.19344587270295371),
	vec3(0.551331865185163, 0.3254901960784313, -0.7681727062895907),
	vec3(0.11267476214340047, 0.3176470588235294, 0.9414907030856382),
	vec3(-0.7210158549990362, 0.30980392156862746, -0.6198045393676209),
	vec3(0.9528452261498945, 0.3019607843137255, -0.030094181165070736),
	vec3(-0.6840226944697828, 0.2941176470588235, 0.6675385855056467),
	vec3(0.05359424873411084, 0.28627450980392155, -0.956647563912202),
	vec3(0.6081491172151762, 0.2784313725490196, 0.7433912980462382),
	vec3(-0.9527998764677731, 0.2705882352941177, -0.13767498800948358),
	vec3(0.7973564495088168, 0.26274509803921564, -0.5433117943529818),
	vec3(-0.22142523557655253, 0.2549019607843137, 0.9412735284911503),
	vec3(-0.4735428503207295, 0.24705882352941178, -0.8454106142144078),
	vec3(0.9221094718213734, 0.23921568627450984, 0.3041216490444623),
	vec3(-0.8870974227959627, 0.2313725490196078, 0.3994056910322261),
	vec3(0.38504706658688165, 0.22352941176470587, -0.8954179798222516),
	vec3(0.3215057412749366, 0.21568627450980393, 0.9220159918978277),
	vec3(-0.8613777366443282, 0.207843137254902, -0.46349716839430055),
	vec3(0.9498247269102629, 0.19999999999999996, -0.24048490212452953),
	vec3(-0.5387870162564602, 0.19215686274509802, 0.8202342904398855),
	vec3(-0.15701567097616714, 0.1843137254901961, -0.9702445720867644),
	vec3(0.7722978854173965, 0.17647058823529416, 0.6102574109895816),
	vec3(-0.9830616785293202, 0.1686274509803921, 0.07179497881447364),
	vec3(0.6772810091416928, 0.16078431372549018, -0.7179406933137539),
	vec3(-0.014462198100436497, 0.15294117647058825, 0.9881294658929549),
	vec3(-0.6575934703608431, 0.14509803921568631, -0.739268142661745),
	vec3(0.9853700592433218, 0.13725490196078427, 0.10102939292378196),
	vec3(-0.795672287983962, 0.12941176470588234, 0.5917416710779092),
	vec3(0.1871753085999864, 0.1215686274509804, -0.9747750882486609),
	vec3(0.5209210551855856, 0.11372549019607847, 0.8459951342318688),
	vec3(-0.9564058417478666, 0.10588235294117643, -0.2721705222947461),
	vec3(0.8897912039565987, 0.0980392156862745, -0.44571282856687755),
	vec3(-0.35529417189270485, 0.09019607843137256, 0.9303927766565466),
	vec3(-0.36673836345332517, 0.08235294117647063, -0.9266719839571937),
	vec3(0.8969343860988999, 0.07450980392156858, 0.43584056276688954),
	vec3(-0.9563095282490215, 0.06666666666666665, 0.284653546852467),
	vec3(0.5131256337870157, 0.05882352941176472, -0.8562954375320568),
	vec3(0.2001428096230549, 0.050980392156862786, 0.978439500108066),
	vec3(-0.8088045974064382, 0.04313725490196074, -0.5864932228540519),
	vec3(0.9928636252813212, 0.03529411764705881, -0.11391289151254008),
	vec3(-0.6553210741089532, 0.027450980392156876, 0.7548514645307363),
	vec3(-0.026686399877644276, 0.019607843137254943, -0.9994515338669882),
	vec3(0.6948830397302775, 0.0117647058823529, 0.7190265313538237),
	vec3(-0.9981419739864699, 0.0039215686274509665, -0.060804778314647),
	vec3(0.7770718651175517, -0.0039215686274509665, -0.6293996645552249),
	vec3(-0.14782615027404392, -0.0117647058823529, 0.9889433861403076),
	vec3(-0.5589504666630543, -0.019607843137254832, -0.8289691841707137),
	vec3(0.9719338330108055, -0.027450980392156765, 0.23364731525192647),
	vec3(-0.8742848861151504, -0.03529411764705892, 0.48412835402415155),
	vec3(0.317548624558525, -0.04313725490196085, -0.9472602853917776),
	vec3(0.4055646042393056, -0.050980392156862786, 0.9126436059074472),
	vec3(-0.9151372708370478, -0.05882352941176472, -0.39882774215500977),
	vec3(0.9437316257842705, -0.06666666666666665, -0.32392309897587307),
	vec3(-0.476806045152912, -0.07450980392156858, 0.8758448974705558),
	vec3(-0.23989425679180534, -0.08235294117647052, -0.9672997149993854),
	vec3(0.8297262711391887, -0.09019607843137245, 0.5508348050160402),
	vec3(-0.9831653730125441, -0.0980392156862746, 0.15418872039395445),
	vec3(0.6203010922626137, -0.10588235294117654, -0.7771843296628212),
	vec3(0.06753085664392451, -0.11372549019607847, 0.9912144552419518),
	vec3(-0.7186781232937122, -0.1215686274509804, -0.6846333499901335),
	vec3(0.9914021661552934, -0.12941176470588234, 0.019347870639982685),
	vec3(-0.7433065845682624, -0.13725490196078427, 0.6547185755919867),
	vec3(0.10571559384638769, -0.1450980392156862, -0.9837534102779358),
	vec3(0.5858637637063266, -0.15294117647058814, 0.7958471253423298),
	vec3(-0.9683624975888785, -0.1607843137254903, -0.19084673883316028),
	vec3(0.8418369091249076, -0.16862745098039222, -0.5127137614799092),
	vec3(-0.27402866435333406, -0.17647058823529416, 0.9453922057012195),
	vec3(-0.4359050928365134, -0.1843137254901961, -0.8809172495956219),
	vec3(0.9150722071315112, -0.19215686274509802, 0.3545681822096087),
	vec3(-0.9127920564370979, -0.19999999999999996, 0.356104846506382),
	vec3(0.43179789776221106, -0.2078431372549019, -0.8776968757972056),
	vec3(0.2740045062916658, -0.21568627450980382, 0.9372304740670472),
	vec3(-0.8336224925907797, -0.22352941176470598, -0.5050823120273172),
	vec3(0.9540689151373039, -0.2313725490196079, -0.1903135537183234),
	vec3(-0.5738236287538725, -0.23921568627450984, 0.7832638754110605),
	vec3(-0.10575290205057612, -0.24705882352941178, -0.9632124695123835),
	vec3(0.7270904643363612, -0.2549019607843137, 0.6374672125368072),
	vec3(-0.9646356751629583, -0.26274509803921564, 0.021048222235299913),
	vec3(0.6955066471909497, -0.2705882352941176, -0.6656218976518268),
	vec3(-0.06307677761825406, -0.2784313725490195, 0.9583826432619522),
	vec3(-0.5994231191487821, -0.28627450980392166, -0.7474883472446037),
	vec3(0.944566535699958, -0.2941176470588236, 0.14590705714379068),
	vec3(-0.793015679482818, -0.3019607843137255, 0.5290990614535657),
	vec3(0.22674287102378796, -0.30980392156862746, -0.9233683991888555),
	vec3(0.45528895407719133, -0.3176470588235294, 0.8317525559420051),
	vec3(-0.8950353660439317, -0.3254901960784313, -0.30490625737663957),
	vec3(0.8634264645906224, -0.33333333333333326, -0.3786603083681038),
	vec3(-0.3797473271354591, -0.3411764705882352, 0.8598782375722911),
	vec3(-0.2999026345670632, -0.34901960784313735, -0.8878309090816546),
	vec3(0.8182684716950331, -0.3568627450980393, 0.4506502961176954),
	vec3(-0.9048272343886239, -0.3647058823529412, 0.21972094867307665),
	vec3(0.5170392038121552, -0.37254901960784315, -0.7706346019421446),
	vec3(0.13882912881356824, -0.3803921568627451, 0.9143458207867384),
	vec3(-0.7174581202308888, -0.388235294117647, -0.5783832657642711),
	vec3(0.9163866344686681, -0.39607843137254894, -0.057943182244753866),
	vec3(-0.6342018097941307, -0.4039215686274509, 0.659268860823404),
	vec3(0.02222551502218701, -0.41176470588235303, -0.9110191290371558),
	vec3(0.5966399275113888, -0.41960784313725497, 0.6840687501098889),
	vec3(-0.8983814981436993, -0.4274509803921569, -0.10097694367956649),
	vec3(0.7276165576507156, -0.43529411764705883, -0.5301822103526214),
	vec3(-0.17762906966412542, -0.44313725490196076, 0.8786792855918547),
	vec3(-0.460538542146354, -0.4509803921568627, -0.7645396896746128),
	vec3(0.8521833652439953, -0.45882352941176463, 0.2515243146488047),
	vec3(-0.794597446660883, -0.46666666666666656, 0.3883775482468414),
	vec3(0.32203580880441607, -0.4745098039215687, -0.8192273090113634),
	vec3(0.3143872462449601, -0.48235294117647065, 0.8176162299862443),
	vec3(-0.7802041670205865, -0.4901960784313726, -0.3885733681741665),
	vec3(0.833491179517867, -0.4980392156862745, -0.2392684544701109),
	vec3(-0.4505891415901249, -0.5058823529411764, 0.7355626896898545),
	vec3(-0.16372807015625493, -0.5137254901960784, -0.8421871762059236),
	vec3(0.6858030268724634, -0.5215686274509803, 0.5075828751952411),
	vec3(-0.843739200913543, -0.5294117647058822, 0.0884722794596109),
	vec3(0.5591067462989906, -0.5372549019607844, -0.6314719443982164),
	vec3(0.014199762301504949, -0.5450980392156863, 0.8382520470561314),
	vec3(-0.5731576032314731, -0.5529411764705883, -0.6047697224739966),
	vec3(0.8258993911032322, -0.5607843137254902, 0.05840504477133189),
	vec3(-0.6442414064703604, -0.5686274509803921, 0.5114839510489392),
	vec3(0.12867856485799636, -0.5764705882352941, -0.8069222315971599),
	vec3(0.447104780305894, -0.584313725490196, 0.6772553326710584),
	vec3(-0.7816267140493922, -0.5921568627450979, -0.19598451415413617),
	vec3(0.7036116864169257, -0.6000000000000001, -0.3806975108107876),
	vec3(-0.2597200920661278, -0.607843137254902, 0.7503813658862274),
	vec3(-0.31295675848565097, -0.615686274509804, -0.7231794235861473),
	vec3(0.7136137725506536, -0.6235294117647059, 0.3193218694207835),
	vec3(-0.7358977743221183, -0.6313725490196078, 0.24458775540127786),
	vec3(0.3742713186234645, -0.6392156862745098, -0.6718067329794042),
	vec3(0.17629968942567262, -0.6470588235294117, 0.7417771217833191),
	vec3(-0.625493940060057, -0.6549019607843136, -0.42409993245579186),
	vec3(0.7408992542833064, -0.6627450980392158, -0.10879903504825154),
	vec3(-0.468393881382283, -0.6705882352941177, 0.5752552394970087),
	vec3(-0.042782950354031686, -0.6784313725490196, -0.7334169972807132),
	vec3(0.5217115290418067, -0.6862745098039216, 0.5067981626429274),
	vec3(-0.7195532405099417, -0.6941176470588235, -0.02106718094963054),
	vec3(0.5390201978862116, -0.7019607843137254, -0.4655193696897785),
	vec3(-0.08209197859965624, -0.7098039215686274, 0.6995993853452069),
	vec3(-0.40736539186523646, -0.7176470588235293, -0.564832839406864),
	vec3(0.6739132486360818, -0.7254901960784315, 0.13965997532164223),
	vec3(-0.5840767478303788, -0.7333333333333334, 0.3479605938408976),
	vec3(0.1931731064259431, -0.7411764705882353, -0.64291647233534),
	vec3(0.28803464830097464, -0.7490196078431373, 0.5966621057555498),
	vec3(-0.6070915042092702, -0.7568627450980392, -0.24207166418146986),
	vec3(0.6025696314835725, -0.7647058823529411, -0.22833035870983381),
	vec3(-0.28583857854463235, -0.7725490196078431, 0.5669782353129126),
	vec3(-0.16959844436412916, -0.780392156862745, -0.6018508529331668),
	vec3(0.5231704072567714, -0.7882352941176471, 0.32400284887335506),
	vec3(-0.5946275913773458, -0.7960784313725491, 0.11259288910139004),
	vec3(0.3561418909659425, -0.803921568627451, -0.47631194084841216),
	vec3(0.05806717656643265, -0.8117647058823529, 0.5810905826885682),
	vec3(-0.42709339485055187, -0.8196078431372549, -0.38188246299473827),
	vec3(0.561497120842661, -0.8274509803921568, -0.006771878133918324),
	vec3(-0.4008996606726137, -0.8352941176470587, 0.3762488526185892),
	vec3(0.040545697792686526, -0.8431372549019607, -0.5361675258600528),
	vec3(0.3245536800681917, -0.8509803921568628, 0.41291316389617055),
	vec3(-0.50548007776653, -0.8588235294117648, -0.08313865713290336),
	vec3(0.4176793459716761, -0.8666666666666667, -0.272823849466211),
	vec3(-0.12025253617236542, -0.8745098039215686, 0.4698637359800961),
	vec3(-0.22191798124753145, -0.8823529411764706, -0.41497674247631294),
	vec3(0.42978728549824047, -0.8901960784313725, 0.1511086733760164),
	vec3(-0.40458006244848904, -0.8980392156862744, 0.17274414652531003),
	vec3(0.1748720977711282, -0.9058823529411764, -0.3857420278513647),
	vec3(0.1262753464090207, -0.9137254901960785, 0.3862127204730805),
	vec3(-0.3382114024734945, -0.9215686274509804, -0.19058938096081837),
	vec3(0.3594549091899665, -0.9294117647058824, -0.08358193516263254),
	vec3(-0.19706181144617116, -0.9372549019607843, 0.28761065908626243),
	vec3(-0.045900775715353836, -0.9450980392156862, -0.323547237755778),
	vec3(0.23414654968012927, -0.9529411764705882, 0.19255780290536664),
	vec3(-0.2769016634980678, -0.9607843137254901, 0.014797677217788923),
	vec3(0.1740295571899153, -0.968627450980392, -0.1774107506086095),
	vec3(-0.007362622594429105, -0.9764705882352942, 0.21552489902549668),
	vec3(-0.11466373443661554, -0.9843137254901961, -0.1340847411775856),
	vec3(0.1240747264536609, -0.9921568627450981, 0.015173067035173201),
	vec3(-0.0, -1.0, 0.0),
};

uint quantizeNormal(vec3 v)
{
    float m = sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));

    if (m == 0) {
        return 2 + 2 * 5 + 2 * 25;
    }

    v.x = v.x / m * 2;
    v.y = v.y / m * 2;
    v.z = v.z / m * 2;
    
    int x = int(round(v.x)) + 2;
    int y = int(round(v.y)) + 2;
    int z = int(round(v.z)) + 2;

    return x + y * 5 + z * 25;
}
