/*
 * mp_zeta_test.c
 *
 * Small test suite for 
 * High-precison Riemann zeta function, using the 
 * Gnu Multiple-precision library.
 *
 * Currently not automated, done purely by visual inspection
 *
 * Linas Vepstas July 2005
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <gmp.h>
#include "mp-consts.h"
#include "mp-complex.h"
#include "mp-misc.h"
#include "mp-trig.h"
#include "mp_zeta.h"

/* ==================================================================== */

void fp_pi_string (mpf_t pi)
{
	char *p = "3.1415926535 8979323846 2643383279 5028841971 6939937510"
		   "5820974944 5923078164 0628620899 8628034825 3421170679"
			 "8214808651 3282306647 0938446095 5058223172 5359408128"
			  "4811174502 8410270193 8521105559 6446229489 5493038196"
			   "4428810975 6659334461 2847564823 3786783165 2712019091"
				 "4564856692 3460348610 4543266482 1339360726 0249141273"
				  "7245870066 0631558817 4881520920 9628292540 9171536436"
				   "7892590360 0113305305 4882046652 1384146951 9415116094"
					 "3305727036 5759591953 0921861173 8193261179 3105118548"
					  "0744623799 6274956735 1885752724 8912279381 8301194912"
					   "9833673362 4406566430 8602139494 6395224737 1907021798"
						 "6094370277 0539217176 2931767523 8467481846 7669405132"
						  "0005681271 4526356082 7785771342 7577896091 7363717872"
						   "1468440901 2249534301 4654958537 1050792279 6892589235"
							 "4201995611 2129021960 8640344181 5981362977 4771309960"
							  "5187072113 4999999837 2978049951 0597317328 1609631859"
							   "5024459455 3469083026 4252230825 3344685035 2619311881";
	mpf_set_str (pi, p, 10);
}


/* return e^pi */
void fp_e_pi_string (mpf_t e_pi)
{
	static int inited=0;
	static mpf_t e;

	if (0 == inited)
	{
		inited = 1;
		mpf_init (e);

		char *p= "23.140692632779269005729086367948547380266106242600211993445046409524342350690 452783516971997067549219675952704801087773144428044414693835844717445879609849 365327965863669242230268991013741764684401410395183868477243068059588162449844 491430966778413671631963414784038216511287637731470347353833162821294047891936 224820221006032065443362736557271823744989618858059591684872645479013397834026 595101499643792422968160799565381423536206957600770590460899883002254304871211 791300849327379580729427301931042601691939325853203428968661895283290521711157 185185506802254197204566370865568386830544799278170407497768540367556534957218 867882563994384718224585889428535247260568210271076018491534518468064887386774 439630514005169440540665265430968869063937315359837311042174433023967896690035";
		mpf_set_str (e, p, 10);
	}
	mpf_set (e_pi, e);
}

/* fp_euler
 * return Euler-Mascheroni const
 */
void fp_euler_mascheroni_string (mpf_t gam)
{
	static int inited=0;
	static mpf_t e;

	if (0 == inited)
	{
		inited = 1;
		mpf_init (e);

		// char * g="0.5772156649015328606065120900824024310421593359399235988057672348848677267776646709369470632917467495e0";
	char * g="0.57721566490153286060651209008240243104215933593992359880576723488486772677766 467093694706329174674951463144724980708248096050401448654283622417399764492353 625350033374293733773767394279259525824709491600873520394816567085323315177661 152862119950150798479374508570574002992135478614669402960432542151905877553526 733139925401296742051375413954911168510280798423487758720503843109399736137255 306088933126760017247953783675927135157722610273492913940798430103417771778088 154957066107501016191663340152278935867965497252036212879226555953669628176388";
	
		mpf_set_str (e, g, 10);
	}
	mpf_set (gam, e);
}


void fp_zeta2 (mpf_t zeta)
{
	static int inited=0;
	static mpf_t z;

	if (0 == inited)
	{
		inited = 1;
		mpf_init (z);

		mpf_t pi, pisq;
		mpf_init (pi);
		mpf_init (pisq);
		
		fp_pi (pi, 1000);  // XXX  bad hard-coded prcision value
		mpf_mul (pisq, pi, pi);
		mpf_div_ui (z, pisq, 6);
	
		mpf_clear (pi);
		mpf_clear (pisq);
	}
	mpf_set (zeta, z);
}

void fp_zeta3 (mpf_t zeta)
{
	static int inited=0;
	static mpf_t e;

	if (0 == inited)
	{
		inited = 1;
		mpf_init (e);

		// http://www.worldwideschool.org/library/books/sci/math/MiscellaneousMathematicalConstants/chap97.html
		// char * g="1.202056903159594285399738161511449990764986292";
		char * g="1.2020569031595942853997381615114499907649862923404988817922715553418382057863 130901864558736093352581461991577952607194184919959986732832137763968372079001 614539417829493600667191915755222424942439615639096641032911590957809655146512 799184051057152559880154371097811020398275325667876035223369849416618110570147 157786394997375237852779370309560257018531827900030765471075630488433208697115";
		mpf_set_str (e, g, 10);
	}
	mpf_set (zeta, e);
}

void fp_zeta5 (mpf_t zeta)
{
	static int inited=0;
	static mpf_t e;

	if (0 == inited)
	{
		inited = 1;
		mpf_init (e);

		// http://www.worldwideschool.org/library/books/sci/math/MiscellaneousMathematicalConstants/chap97.html
		char * g="1.036927755143369926331365486457034168057080919501912811974192677 9038035897862814845600431065571333363796203414665566090428009617 7915597084183511072180087644866286337180353598363962365128888981 3352767752398275032022436845766444665958115993917977745039244643 9196666159664016205325205021519226713512567859748692860197447984 3200672681297530919900774656558601526573730037561532683149897971 9350398378581319922884886425335104251602510849904346402941172432 7576341508162332245618649927144272264614113007580868316916497918";

		mpf_set_str (e, g, 10);
	}
	mpf_set (zeta, e);
}

void fp_zeta7 (mpf_t zeta)
{
	static int inited=0;
	static mpf_t e;

	if (0 == inited)
	{
		inited = 1;
		mpf_init (e);

		// http://www.worldwideschool.org/library/books/sci/math/MiscellaneousMathematicalConstants/chap97.html
		char * g="1.008349277381922826839797549849796759599863560565238706417283136 5716014783173557353460969689138513239689614536514910748872867774 1984033544031579830103398456212106946358524390658335396467699756 7696691427804314333947495215378902800259045551979353108370084210 7329399046107085641235605890622599776098694754076320000481632951 2586769250630734413632555601360305007373302413187037951026624779 3954650225467042015510405582224239250510868837727077426002177100 0195455778989836046745406121952650765461161356548679150080858554";
		mpf_set_str (e, g, 10);
	}
	mpf_set (zeta, e);
}

void fp_zeta9 (mpf_t zeta)
{
	static int inited=0;
	static mpf_t e;

	if (0 == inited)
	{
		inited = 1;
		mpf_init (e);

		// http://www.worldwideschool.org/library/books/sci/math/MiscellaneousMathematicalConstants/chap97.html
		char * g="1.002008392826082214417852769232412060485605851394888756548596615 9097850533902583989503930691271695861574086047658470602614253739 7072243015306913249876425109092948687676545396979415407826022964 1544836250668629056707364521601531424421326337598815558052591454 0848901539527747456133451028740613274660692763390016294270864220 1123162209241265753326205462293215454665179945038662778223564776 1660330281492364570399301119383985017167926002064923069795850945 8457966548540026945118759481561430375776154443343398399851419383";

		mpf_set_str (e, g, 10);
	}
	mpf_set (zeta, e);
}

/* ==================================================================== */
/* 
 * From http://pi.lacim.uqam.ca/piDATA/zeta100.html
 * Non trivial zeros of the Riemann Zeta function,
 * to over 1000 decimal digits each.  We consider here the Imaginary
 * part of 1/2 + iy = z, for which Zeta(z) is a zero.
 * 
 * These values were computed by Andrew M. Odlyzko, AT&T Labs - Research.
 * To see more tables of zeros and related papers, see
 * <A HREF="http://www.research.att.com/~amo">his home page.</a>
 */

static void zeta_zero (mpf_t zero, int k)
{
	char *zp[20];
	zp[0] = " \
  14.134725141734693790457251983562470270784257115699243175685567460149 \
 9634298092567649490103931715610127792029715487974367661426914698822545 \
 8250536323944713778041338123720597054962195586586020055556672583601077 \
 3700205410982661507542780517442591306254481978651072304938725629738321 \
 5774203952157256748093321400349904680343462673144209203773854871413783 \
 1735639699536542811307968053149168852906782082298049264338666734623320 \
 0787587617920056048680543568014444246510655975686659032286865105448594 \
 4432062407272703209427452221304874872092412385141835146054279015244783 \
 3835425453344004487936806761697300819000731393854983736215013045167269 \
 6838920039176285123212854220523969133425832275335164060169763527563758 \
 9695376749203361272092599917304270756830879511844534891800863008264831 \
 2516911271068291052375961797743181517071354531677549515382893784903647 \
 4709727019948485532209253574357909226125247736595518016975233461213977 \
 3160053541259267474557258778014726098308089786007125320875093959979666 \
 60675378381214891908864977277554420656532052405"; 
 
	zp[1] = " \
  21.022039638771554992628479593896902777334340524902781754629520403587 \
 5985860688907997136585141801514195337254736424758913838650686037313212 \
 6211882162437574166925654471184407119403130672564622779261488733743555 \
 2059147397132822662470789076753814440726466841906077127569834054514028 \
 4399232225367882682361112892700575856532731588666042140009071151080090 \
 0697200279987110175847519632216496865900574811247938691638351837234278 \
 0734490239101038504575641215958399921001621834669113158721748057170315 \
 7935817977249632724076992211256634415618236051804767144227146555596737 \
 8124776500455584090864429169775704638165517749644524987674237036645657 \
 7704837992029270664315837893238009151146858070430828784147861992007607 \
 7604774841407827389070038957604332451278278637209093037972518237091808 \
 0423066673834379902282515828788761761266187138296785874562376500666242 \
 0780814517636976391374340593412797549697276850306200263121273830462939 \
 3025654143823744333440220248004533438830728387312602306547534837868011 \
 82789317520010690056016544152811050970637593228";

	zp[2] = " \
  25.010857580145688763213790992562821818659549672557996672496542006745 \
 0920984416442778402382245580624407504710461490557783782998515227308011 \
 8813393358267168958722516981043873551292849372719199462297591267547869 \
 6628856807735070039957723114023284276873669399873219586487752250099192 \
 4534749762085766123345997354435583675313812659977645290374484969947911 \
 3789772206619930718997232254973227163005159161921279774087660006729149 \
 8308127930667027350849516001984670542469491796695225514179319665391273 \
 4145216731602337377544894146417119378489574997514110658562879690076709 \
 8628272186495372963239258403491387143048933588946114958624239036855617 \
 5189359878735685683089271444468756375337019130417377142535868018531867 \
 8963753268686326607197669205329533478506707982877118674944281439725425 \
 5165319679779912722684458969279408599507227960513612021369680647653397 \
 6269691774251249095257214003855886494422730332216278403670865759210329 \
 0789866156020484275192735141927597017849166084411074821559128310749314 \
 22640278339513428773126644105168571016344289902";
 
	zp[3] = " \
  30.424876125859513210311897530584091320181560023715440180962146036993 \
 3293893332779202905842939020891106309917115273954991176332266711863193 \
 9180722595671424334115590685468136558072417349844724959319040811632315 \
 0197023484841630221400985620739718392018133021868063298225719752250023 \
 7468561369747124964426229779245040574906715345727886515065160832468797 \
 0628177810457777225878919237386290011276030973568089049253006461289272 \
 7530919447902003589389819427495511323917384271638108400499211198006924 \
 3871887296959700029100054774270689081684625934838507707996560373392659 \
 1631785900558390596815720730796252620549400959515892318195507003120438 \
 5472912847073737931700052460469858203860095171051337905912538151203525 \
 6495480686539474573064428698419890124742762009249476736375814720332208 \
 6687601457265777407119672734350479234503516187981145579444869326121291 \
 4417916583251901867849867644777729648215979712565041026341481014213352 \
 4013338332668144856154491448771220118284070765164762211312808070237683 \
 31017097022722833154052850963731871619582513781";
	
	zp[4] = " \
  32.935061587739189690662368964074903488812715603517039009280003440784 \
 8156086305510059388484961353487245496025252805975815135792377828577545 \
 0603765301147268210982527271365947816607918650788117035383676547460173 \
 8548120651787886596466594728787186027971658042677648544066692909393193 \
 1156455083917513007902799895626566839200248741651699908688450128764042 \
 5095606414489302377479705325278240994775176594460744680987406706533829 \
 0958915614204800224198408375141584435237806584753908207901246070504560 \
 1001022574389668364953842377042049095598981489088287256372979657214165 \
 2631778575520115507778405795014772072579821404213096863779363536418594 \
 0239585699090026826663214819343540824642411198675065226034566669947805 \
 3849257790018375924514618207385743601378106373715610378620890222774133 \
 2041773464342362863962973628444554246794670572412945536985966772759610 \
 7799501092598501136571980327345054062391125982107996408750156764769596 \
 4194638473048004511557288836606508923318034977150164443483206561234793 \
 40307063230555187773781024210561808202609296218";

	zp[5] = " \
  37.586178158825671257217763480705332821405597350830793218333001113622 \
 1490896185372647303291049458238034745777477461922353799409650222639362 \
 8562482204748368808900366789035453755377790319090564409937583872127563 \
 4312643051507490897122077467801551204327992986651432331546548850586795 \
 0603413625492599668820961532811560379952622032357848783110737761462301 \
 4910557937901567716341125126537129565017674488645214983270201198449431 \
 5326208033153177607464795075040560820830213633950862288220551335769555 \
 8035937464815987090800740796272177164722740705738059683514846795303217 \
 2760871124006737943183367067907130787878538857435773069511259743788562 \
 7621434891374075997674405555785944574435456054998502810248039697965044 \
 3332124660412703406031513408241203980669209295582721330805747422403795 \
 2694144756038717139141772282386227733350407525960009283974067845039605 \
 5572399332457099842085439308578390827058000428133328048210695855943079 \
 8354229722750553495935631680975274173043012765178378868858782566744124 \
 56445093066944548079936612500679148846677611635";
 
	zp[6] = " \
  101.31785100573139122878544794029230890633286638430089479992831871523 \
 0143082729312355508904945205971856606279547624688363814627931506495066 \
 1417608124330919538238884893903926570119170263834018751496529553441220 \
 6365830087649168464626518598267064779746825187820926960056222306419701 \
 0104993623099425679029266967257399641695479426441028504089814718943177 \
 0568080294393310094364145051494796221063795571215325721550534189382914 \
 3390059888796297655338051514690448486649068813074614701115706706725422 \
 3014721672526267754741838391031465779116365319089144521806679054528163 \
 4825706789311310235765676981505374972550754214046499526906619151737359 \
 4849351746570629604204928603072680202737982149328664567447162535749149 \
 0359949625409394170081939672452641260719757809636923325310966144653641 \
 0077488417064322917367676544091747366772239050296978354285801132817604 \
 5628179115847506969995235243562404830232843095615924404234446553656737 \
 0206639404178561780789560787362584550692015911545434547952233252252548 \
 10551193284721748482559527977334395819351122225";
  
	zp[7] = " \
  103.72553804047833941639840810869528083448117306949576451988516579403 \
 6522108301648805438145498492016894600891106996629226791146592306589237 \
 0422525139321773948328911985490537053203394829453494851131619314716793 \
 1801001680834226178580626626599823521973054397696858452812247345802777 \
 1854637762040827450964417638062509616005048100437719022593828282671312 \
 4676864272075309830612385095287802180387707903195064767721267295766128 \
 8403974129219807717255773913554278978887694816019652335267170755381863 \
 3118469094867503218289350439645051736659911245972537701624584999770456 \
 5062748838212167024071916066120160626255579007402022300148249840509997 \
 2858361804949871793055850294039145395904789602410545817473574047064254 \
 2993331835817879247687995603165720495275804799041911007215937923564139 \
 6376838761582413693985722895663023054886478145082893516248420925495479 \
 2677949479957450018023614180524583700107010331375974459845150758554994 \
 6071345506415657116138223829473877123421248508443746546157265660993368 \
 10040644930213234365990073855542458653893947561";
 
	zp[8] = " \
  105.44662305232609449367083241411180899728275392853513848056944711418 \
 1494447575962839773554931962814515480735223511489685170485682218940337 \
 2030891033165259692831161764636231865286355323428429969940737804080880 \
 4168216185145562162092815512623294797442117366143278357688322959110923 \
 8943632292560243764987672271414515274633253469083085416223603986996110 \
 9352589746503160720546633850831807327935840524119718742655282802495159 \
 4947081417022226627887349297932991078053388355633040322273847655839381 \
 1475167287851801380712129780438852761863361271255228514885657600372075 \
 3336739186858673327762480856098765497998427180455945372460039788675270 \
 9556733553072687456105716032568590390912624457466971183186222702405489 \
 0292569250685033404947428241585812619270366779349837794807112887305361 \
 4436503462104427511888836662229664398060467156964134163748417512733988 \
 3016159856686826941364109233810820811457787047082284476809086868447671 \
 9191160203130997109080928554149891729293934541248464866435624402037121 \
 55718531952992295774004038490270855445232703343";
 
	zp[9] = " \
  231.25018870049916477380618677001037260670849584312337140680603034414 \
 0047722005275297107726867291239984314549185611962233738127940815362089 \
 4368814456065586418251627365438772967141098166971086057766271230605178 \
 8610996085630665941938128629649207945126859151807174304072745505534742 \
 1518478633195850406070517080484989876013817412193948687938788449439878 \
 9382981264211472707886059122965769078523047652832198184130289953516224 \
 8931753558907072963407280932766695861095834417179542037317984772839786 \
 3319504684579274709538619156216026824382622197940544616303272783118771 \
 2810459051659040112723392671704359278509188052556564381877104028012896 \
 4434104137385863437111308280116774237045555677309381968046004736175583 \
 0715653767362098964566094288633910677965217286003862089115996940125625 \
 4002089748417092304071384204972248753718906447883181557258102234588016 \
 2860352718274303234653427799971763888327923378636820718351619867147443 \
 0199152562958529960013357820186609020345725026777663549619600427481140 \
 98064700216812020741170015838931040128668327422";
 
	zp[10] = " \
  231.98723525318024860377166853919786220541983399456249648472682389683 \
 0763674863891148755524430359243470247636260196277988598029555692481618 \
 6811442966796410907066849365249001069619345428159009167194833946212743 \
 9897602200870190016407542534804877333742559351572959887365282682990820 \
 6672844529874079019926494508602098427869406218160537448445500157840068 \
 2551612228512160870983291556500105976129464863866854903052728907051384 \
 7208774179629562314996560797322263345030333307205452163610850962791309 \
 3388688613972105928273614396956798142983909248449243914601082074071657 \
 6657772414534056523749830953245299228105737890787437034529953500569637 \
 0639691815655240154793168205725574738779586677844983063766912832811200 \
 1562179888983307250271166607950429723727675711987946284644700009815849 \
 0831625128070188582717625022171395536608284546429600652360379370602083 \
 7796185897060277298349554190279919536418693029198830877804883634584615 \
 3007321188976083853577821526855947207127363234295980453425340867137129 \
 46902968651314477613688963498143666844069697484";
  
	zp[11] = " \
  233.69340417890830064070449473256978817953722775456583636301480873894 \
 0154701248547362941997285322469889631102282082522926752244730132449471 \
 7340819884986676410242516761258606472858752363043751148910506095571681 \
 6268672484623330649977745923174519142071699413002316603920348406431124 \
 3323763191904302980133034540100074725009289959246678743580081455994319 \
 1828379761461681638207631074940394851853891787407262975159834875141152 \
 0770640662589348083842110165083173087755595821010985244821907710780768 \
 7922865351874625906979342078734170713012434645528736863984031905859477 \
 9566910153774270806071317098407926546903143404180066918064045788431467 \
 6329292693035462839132112342773367257006826015928022870250950036431051 \
 2619707611003499577595928838248272284707459824033821845215208487471787 \
 1864509337694866470779251003674342559537531972643313765569895760577896 \
 6497101855471906017522063295574409062466395578250384902326017330131825 \
 9340477803173737115408536099456014154394252756554609149017830897672667 \
 52789999139962465789899130970561104489398394494";
  
	zp[12] = " \
  236.52422966581620580247550795566297868952949521218912370091896098781 \
 9150384292332826261444604065174015827315678371311908125834666026514272 \
 3427442710559412695531311686041161208386105258022537008648905356952178 \
 6290474725602526826563480477997212116546341401730768688207342088012792 \
 6644985268669489487216328558298806627153861486608112087260596055996397 \
 4174141107331932566901602812030379738385137490926579562946886420201539 \
 1274403467040835271042467991402807619705981381817538187168651059206275 \
 5648099062015611059642060030641789502508561511728245696694558507994532 \
 1497441545297219126705861738514169501070389572158914104764623371280124 \
 8443401149192190622936151728642860733202107967849957639411015846947971 \
 7452713139834298578033384520966674050617154905576061284758618968479553 \
 2764561894709395106683170789220436322873140268304456973831742846612049 \
 2681054686977186813195678067420928919099149530103114468675907950861633 \
 3258478241372746723689425816666668066628696207830016767930784349540940 \
 46048236402952743224761478361793490446024555390";
  
	if ((k>12) || (k<0))
		return;

	mpf_set_str (zero, zp[k], 10);
}
/* ==================================================================== */

int main (int argc, char * argv[])
{
#ifdef FACT_TEST
	char str[4000];
	mpz_t fact;
	mpz_init (fact);

	i_factorial (fact, 5);
	mpz_get_str (str, 10, fact);
	printf ("fact = %s\n", str);
#endif

#ifdef I_BINOMIAL_TEST
	int n, k;
	mpz_t bin;
	mpz_init (bin);

	for (n=1; n<7; n++)
	{
		for (k=0; k<=n; k++)
		{
			i_binomial (bin, n ,k);
			mpz_get_str (str, 10, bin);
			printf ("bin (%d %d) = %s\n", n, k, str);
		}
		printf ("---\n");
	}
#endif

// #define I_STIRLING_TEST
#ifdef I_STIRLING_TEST
	int n, k;
	mpz_t sitrly;
	mpz_init (sitrly);

	for (n=0; n<21; n++)
	{
		for (k=0; k<=n; k++)
		{
			i_stirling_first (sitrly, n ,k);
			mpz_get_str (str, 10, sitrly);
			printf ("sitrly (%d %d) = %s\n", n, k, str);
		}
		printf ("---\n");
	}
#endif

// #define F_BINOMIAL_TEST
#ifdef F_BINOMIAL_TEST
	int n, k;
	mpf_t bin;
	mpf_init (bin);

	for (n=1; n<7; n++)
	{
		for (k=0; k<=n; k++)
		{
			fp_binomial (bin, (double)n ,k);
			printf ("bin (%d %d) = ", n, k);
			mpf_out_str (stdout, 10, 60, bin);
			printf ("\n");
		}
		printf ("---\n");
	}
#endif
	
	if (argc < 3)
	{
		fprintf (stderr, "Usage: %s [ndigits] [nterms]\n", argv[0]); 
		exit (1);
	}

	/* the decimal precison (number of decimal places) */
	int prec = atoi (argv[1]);

	/* number of an's to compute */
	int nterms = atoi (argv[2]);

	/* compute number of binary bits this corresponds to. */
	double v = ((double) prec) *log(10.0) / log(2.0);

	/* the variable-precision calculations are touchy about this */
	/* XXX this should be stirling's approx for binomial */ 
	int bits = (int) (v + 300 + 3*nterms);
	
	/* set the precision (number of binary bits) */
	mpf_set_default_prec (bits);
	
// #define TEST_LOG
#ifdef TEST_LOG
	mpf_t lg;
	mpf_init (lg);
	mpf_set_ui (lg, 1);
	mpf_div_ui (lg, lg, 2);
	fp_log (lg, lg, prec);
	fp_prt ("log 2= ", lg);
	mpf_clear (lg);
#endif
	
// #define TEST_PI
#ifdef TEST_PI
	mpf_t pi, pis;
	mpf_init (pi);
	mpf_init (pis);
	fp_pi (pi, prec);
	fp_pi_string (pis);
	fp_prt ("pi= ", pi);
	mpf_sub (pi, pi, pis);
	fp_prt ("diff= ", pi);
	mpf_clear (pi);
	mpf_clear(pis);
#endif
	
// #define TEST_EULER
#ifdef TEST_EULER
	mpf_t gam, gams;
	mpf_init (gam);
	mpf_init (gams);
	fp_euler_mascheroni (gam, prec);
	fp_euler_mascheroni_string (gams);
	fp_prt ("gamma= ", gam);
	mpf_sub (gam, gam, gams);
	fp_prt ("diff= ", gam);
	mpf_clear (gam);
	mpf_clear(gams);
#endif
	
// #define TEST_ZETA_INT
#ifdef TEST_ZETA_INT
	mpf_t zeta, zetas;
	mpf_init (zeta);
	mpf_init (zetas);
	fp_zeta5 (zetas);
	fp_zeta (zeta, 5, prec);
	fp_prt ("zeta= ", zeta);
	mpf_sub (zeta, zeta, zetas);
	fp_prt ("diff= ", zeta);
	mpf_clear (zeta);
	mpf_clear(zetas);
#endif

#ifdef TEST_EXP
	mpf_t ex, one;
	mpf_init (ex);
	mpf_init (one);
	mpf_set_ui(one, 1);
	fp_exp (ex, one, 50);
	fp_prt ("e= ", ex);
	mpf_clear (ex);
	mpf_clear(one);
#endif

// #define TEST_SINE
#ifdef TEST_SINE
	mpf_t ex, pi2, pi2n;
	mpf_init (ex);
	mpf_init (pi2);
	mpf_init (pi2n);
	fp_pi (pi2, prec);
	mpf_div_ui (pi2, pi2, 2);
	mpf_set (pi2n, pi2);
	
	int n;
	for (n=1; n<130; n++)
	{
		fp_sine (ex, pi2n, prec);
		if (1==n%4)
		{
			mpf_sub_ui (ex,ex,1);
		}
		if (3==n%4)
		{
			mpf_add_ui (ex,ex,1);
		}
		printf ("sin(%d pi/2)= ", n);
		fp_prt ("", ex);
		printf ("\n");

		fp_cosine (ex, pi2n, prec);
		if (0==n%4)
		{
			mpf_sub_ui (ex,ex,1);
		}
		if (2==n%4)
		{
			mpf_add_ui (ex,ex,1);
		}
		printf ("cos(%d pi/2)= ", n);
		fp_prt ("", ex);
		printf ("\n");
		printf ("\n");

		mpf_add (pi2n, pi2n, pi2);
	}

	mpf_clear (ex);
	mpf_clear(pi2);
	mpf_clear(pi2n);
#endif
	
// #define ZETA_STUFF
#ifdef ZETA_STUFF
	mpf_t zeta;
	mpf_init (zeta);
	
	printf ("           000000000011111111112222222222333333333344444444445555555555666666666677777777778\n");
	printf ("           012345678901234567890123456789012345678901234567890123456789012345678901234567890\n");
	fp_zeta (zeta, 2, 13);
	fp_prt ("13 digs= ", zeta);
	fp_zeta (zeta, 7, 30);
	fp_prt ("30 digs= ", zeta);
	fp_zeta (zeta, 7, 40);
	fp_prt ("40 digs= ", zeta);
	fp_zeta (zeta, 7, 50);
	fp_prt ("50 digs= ", zeta);
	fp_zeta (zeta, 7, 60);
	fp_prt ("60 digs= ", zeta);
	fp_zeta (zeta, 7, 70);
	fp_prt ("70 digs= ", zeta);
	fp_zeta (zeta, 7, 80);
	fp_prt ("80 digs= ", zeta);
#endif

// #define TEST_ZETA
#ifdef TEST_ZETA
	mpf_t zeta;
	mpf_init (zeta);
	// fp_zeta_odd (zeta, 3, 180, 7, 360, 0, 60); 
	// fp_prt ("duude zeta3= ", zeta);
	int i;
	int pr = prec;
	for (i=3; i<nterms; i++ ) {
		fp_zeta (zeta, i, pr);
		printf ("char * zeta_%d_%d = \"", i, pr);
		mpf_out_str (stdout, 10, pr, zeta);
		printf ("\";\n");
		fflush (stdout);
	}
#endif

// #define TEST_COMPLEX_ZETA
#ifdef TEST_COMPLEX_ZETA
	/* this test was passing in Nov 2006 */
	mpf_t nzeta;
	mpf_init (nzeta);
	
	cpx_t zeta, ess;
	cpx_init (&zeta);
	cpx_init (&ess);
	mpf_set_ui (ess.im, 0);

	int i;
	int pr = prec;

	/*  verify that its zero where it should be */
	mpf_set_d (ess.re, 0.5);
	for (i=0; i<13; i++ ) {
		zeta_zero (ess.im, i);
		fp_borwein_zeta_c (&zeta, &ess, pr);
		printf ("zeta(rho[%d]) = ", i);
		cpx_prt (" ", &zeta);
		printf ("\n");
	}

	/* compare values to integer zeta routine */
	mpf_set_ui (ess.im, 0);
	for (i=3; i<nterms; i++ ) {
		fp_zeta (nzeta, i, pr);
		
		mpf_set_ui (ess.re, i);
		fp_borwein_zeta_c (&zeta, &ess, pr);

		mpf_sub (zeta.re,zeta.re, nzeta);

		/* should print zero more or less */
		printf ("zeta(%d)_%d = ", i, pr);
		cpx_prt ("", &zeta);
		printf ("\n");
		fflush (stdout);
	}
#endif

#ifdef TEST_DIGIT_COUNT
	mpz_t ival, tmpa, tmpb;
	mpz_init (ival);
	mpz_init (tmpa);
	mpz_init (tmpb);
	mpz_set_ui (ival, 3000000);
	int nd = num_digits (ival, tmpa, tmpb);
	printf ("found %d digits\n", nd);
#endif

// #define TEST_BERNOULLI
#ifdef TEST_BERNOULLI
	mpq_t bern;
	mpq_init (bern);
	int n = 4;
	for (n=8; n<30; n++) 
	{
		q_bernoulli (bern, n);
		printf ("bernoulli (%d)= ", n);
		mpq_out_str (stdout, 10, bern);
		printf ("\n");
	}
#endif /* TEST_BERNOULLI */

// #define TEST_STIELTJES
#ifdef TEST_STIELTJES
	mpf_t stie;
	mpf_init (stie);
	int i;
	for (i=0; i<40; i++ ) {
		stieltjes_gamma (stie, i);
		printf ("gamma[%d] = ", i);
		mpf_out_str (stdout, 10, 60, stie);
		printf (";\n");
		fflush (stdout);
	}
#endif

// #define TEST_B_SUB_N
#ifdef TEST_B_SUB_N
	mpf_t rbs, ibs, bs;
	mpf_init (rbs);
	mpf_init (ibs);
	mpf_init (bs);
	int i;
	for (i=0; i<40; i++ ) {
		double re_s = i;
		double im_s = 0.0;
		b_sub_s_d (rbs, ibs, re_s, im_s, prec, nterms, -1.0);
		b_sub_n (bs, i, prec);
		mpf_sub (bs, bs, rbs);
		printf (" res=%g ", re_s);
		mpf_out_str (stdout, 10, 60, bs);
		printf (";\n");
		fflush (stdout);
	}
#endif

#define HANDY_COMPLEX_ZETA
#ifdef HANDY_COMPLEX_ZETA
	mpf_t nzeta;
	mpf_init (nzeta);
	
	cpx_t zeta, ess;
	cpx_init (zeta);
	cpx_init (ess);
	mpf_set_ui (ess[0].im, 0);

	int i;
	int pr = prec;

#if HANDY
	mpf_set_ui (ess[0].im, 0);
	mpf_set_d (ess[0].re, 0.5);
	fp_borwein_zeta_c (zeta, ess, pr);
	cpx_prt ("zeta(1/2) = ", zeta);
	printf ("\n");
#endif

	/* zero offsets */
	mpf_set_d (ess[0].re, -0.5);
	for (i=0; i<13; i++ ) {
		zeta_zero (ess[0].im, i);
		fp_borwein_zeta_c (zeta, ess, pr);
		printf ("zeta(rho[%d]-1) = ", i);
		cpx_prt (" ", zeta);
		printf ("\n");
		fflush (stdout);
	}
#endif

	return 0;
}

/* =============================== END OF FILE =========================== */

