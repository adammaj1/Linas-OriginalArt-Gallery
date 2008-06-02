/* 
 * hurwitz-euler.c
 *
 * Measure the performance of computing the Hurwitz zeta function 
 * for arbitrary complex argument. Compare to known-good values
 * so as to make sure we aren't mis-computing.
 *
 * Linas Vepstas Feb 2007
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/times.h>
#include <stdint.h>
#include <unistd.h>


#include <gmp.h>
#include "mp-binomial.h"
#include "mp-cache.h"
#include "mp-complex.h"
#include "mp-misc.h"
#include "mp-polylog.h"
#include "mp-trig.h"
#include "mp-zeta.h"

/* =========================================================== */

/* The below are high-precision values for 
 * the Hurwitz zeta, for q=0.2 and s= 0.5 +i 14.134725;
 */
// #define HIPREC_FOR_0_POINT_2
#ifdef HIPREC_FOR_0_POINT_2
char * restr= "-0.265485138108182132880319284484865009935202615366779771706248710874871696549287213894510836062995589626200348736631321362204510163037882523906511561926814483060487037227057909977364607839515406910052176137749241322966307863765732120413866650404322895564465467652280353362313003588099740225024043725302748081653206235810313614231226231053974596605040262168605727587938209352665884265789949856395453429855557163433416555734314510820653270551394534003697815621687923950709915187600943313614302333453736216731526059574457395904065137743022610380825768126750078867674449867025425876901324054446830320483654211327462639367801431036926068158912532776423276691271451271631946476044131693020692058976469082763267783556698830223701230954564576322042045623848559810814603025907607307527305429966109327036888510970645582235939131168191515218692338783679293510583767105071488590917527116448387786779201330546212967356407643972368081094646905390525179633445245330686126677460510650279119319671831554656492661812742144545199522169108453912961264902416194830729202456110971496636439486579336702493261781660140397939436553201159247196675254635773868627491050258630377467214343841126684179902083769014714795618747888527337372496330014023775111950296369326392605387729477606036097606860329058931107847042920742539461778506174843779540392406398443072820969983775610153701291332315580654293087577667517290517581887204723096420531497840700357527438211216344991655402413411949953629795838064353455069895430925002165915441657747600701400788211203096000580687519886046535011746263419171242402170650134409755326534247304150688300546743777667481254375378778349419047201208227572752403210016152971252785324117081225328514504883889223808703354778029878684607715760878540397169841814451804974248314401952361826514377612969534830359893910740141716422389125814697666698128539570096741725378028678755852990010068899076789942322986291126037184193388442902614132665178006215825431383713749836309194293185317259069005142038602244433191620057359172750124035451193149211105265790389899269009667413587708164376188363783837927981522156561654680315604250622061988714739836437475555604610708158229271391321787354111793228912578243583740222933996570883934016002377394034355802781078015689016354030535300292712566024432481550830952614568641689867370021750715286718781464872173277598749602029344191624682176303822698137706939155804740952322769531235040755521310963840924670220509219082829678961765139172733314181743836240982609642519301357941580777850461249812949459318275590038058570264486531241164520495661579286498527054260173544430290065751873614139569984617979821451126404246456283847593352211063747196724646853088477926752444166927648185503822044946507547900245402424077697784256098156454379997555393844762689500628659714137543361881246435009086834464059905437087228324841307792764338530007980262293321550259394431287862349081293363259874174620346430491350411530558281201067539320030004713937073251192419044825218813195076842340184266247686858444655943340245491473335685845895068728605336279316370573051627826076597179746093269351078044735685606897960584834954410451579600221688391515235834506176194893826195419517316053548202851700969502471824018613716332120241970763738568823195563348077601359779233379645416855248224867841971684599243539388887201252236776045004527905534991836694302410599013911270897279735579788306309710096425401643691363430713514736271357203522560329245392148931305761227170365174589822545686853744003109154359173421775194784880874492696681011166426294260083269026956120625639076999149537699306437421061635332566488713827031281188594505745366770832493826803414578599216314680901712829076351317864877719214569730080781608287916930952211667201936848947124361309749875544623473128979206166736255560265354448176532262324026676957220559605549886976201426004243648362692593678886255632839952450728473390571501308497464081684472991551943749794475864355103091790478111517693121897160170012039556198644315192953461314268226444818745753872410846094020634836830468719164783060652171598766989397315732131901931404510448130021347333774314080781508680585497631275517873928032722593819884567057912e1";
char * imstr= "-0.123977332887618285981574358103515947071033761725288275280749094336212447319970331244280392768200400012262084750655952743973371830445238669788386698436981436273667560499207332272502194330761902272772348826473976862621158594563058705427643337639408906927842916650016785478011664711472348042156688052034609189855246064476432897388697848188119130139552927213298748977911457411989286441310509612430854758496139109196468452632837330912276846830729519564749888639755230180185822116280605183961522953200719721476568190953893541666178823714493604613844997042133067800046890236533779956919359836733613075535674100220373956512305380073812532622984246789109547303898134291003146199843034694798242279144838372919783815366765211833479340804064987892884494252622078436993555327149556169737148246186245205220664875057038302134856162640810254555371541001337418970703089656907172976308113496788467070107299169761905750899323172551414047363386072420944090979374800602640127765043613923278964155798085430702684757231711620263367408454586902035225457523695252265302677265989704624240540598158268629808187996325870411263052952614194514794462177073508503646309240413148502686589014247968506614788273738693335630740686541636893429584181841042859796541637157365922500603937293425788117144026920028665099993613874725751639071010125484580537332931062228687716499734048566052121110728850176358211349865131100513772551715327426528675088070258362854172744223850722992079442481177905877145680606824134662002895722831301340739049248760298862885833267666300451955573917808711418502766426717097436340474382861868122585753591498923350723482212951724308781519837977050266590345654115463406434204764768919913852577298563428667872207286595780493689505498652474484044272925831059651504583755420552245114200718662278109692429742061561439185197308413224943969204699274542224317354547300848277997392282544340486128085577986844875794645104372169657382549150228288465331506874331352884431369098501670048219117647246779207207330864321962547810884722721341363931263881809506575187810202062623980445581506048701867537229888708925451322297514824282580912737956165801112684061453219277038477084329524112016369792328219621072361067645781996116329826748479422742154513109486545859562227471994805797526042464898718759161868210857992277127700633491505926428804818374259836923475266005442110821820568578919491047062978717204859438715674732206070952114424699097459456716035950909601358392551527926943522801546467703194314738396984473804616440486066778637156379783789047195498272048570053099271541077077265132446077123151413475918031788115310595354028952587448411676262368003169747039029886042717891582883917227529937412802112536189324540733385415331999788378547750620914028653562651659235775068252864599777487110332466653160321562628320776256163141703457757947418905631282351479099125304274772143163528762029758985107117508740524409629602256955990285678627277808486655123443621535083520810372319896866724800184177333692445732813250255526391980804991436382559010894705739819841951772883819777142504546613774799259778213443881203980276201755064181216866583121810979678846875193421567586994824329804682092641692046094315922876184175422015614255578041303051962204478229781523146977015700175359391039850481390378829246010562474087944864888855352995833587849674327115274278284629555935836748751084853845038055204291469661304872135913323507906920641511104478381108747669381544433582961300711418066758407042013832469074224058883822412751729624515471585862113373784965249530455639047941244462945485426748026154042861777267986940169133232024231918817845174243822736819738736863535347743344169218936197171780638294652240197524036772208708940744699027996878986653306826036932117293276464036546522385496613982980743163954105829797906047752182624877718459914642919957216061538853636582881207399910107168020916267409741569829284345424445213998591001931807755228648013025998925785624141091734752622383480530422675320978188946781093663963177205726761639986532073672457669225381336963909141048625778840064678027117063872122242565284387209143815381478454988799165868645366099002855737712606888277322418804202513845894577618780032005776919e1";
#endif /* HIPREC_FOR_0_POINT_2 */

/* These are for the Hurwitz zeta, for q=0.3 and s= 0.5 +i 14.134725;
 * and about 4100 decimal digits.
 */
#define HIPREC_FOR_0_POINT_3
#ifdef HIPREC_FOR_0_POINT_3
char * restr= "-0.100243522795423512584298592261206580296259353720043403957070187889307712645906814406241504224737808017815564983813813753331290470914830480131440065453389165307421608022744039819153672107554192765287880938771855496301625726679466111727098505095896441264600658858336170812515985865468970517538702210338785374942011048294676317180869865061981416615921554307494486435676427349348387854801304323357668930586482384105004306594730257442846420988754280528164647286343769810115594183788566377770278357913129527823723169148591225321823045323812967346904594056685773842743892362311622345468747509979176594011252287583610981641586394708398204600455247001016797367844846456491844362920212850103200033379329471199322119523245067761979013487188252163808515282105651416301520357271988684044917807829052932609952952277230799201235681861923269763913276100670349271640637845749468934786314135702595151749882570822661118453405834501131719348962182546483530330919895263687968149988054831654035831113126193678566478205439908365097081522513447124545317300502112612283485854276642091378064164092479069084818555214960229029058558311358684086497469142540330621171093654688715121373863384695765969408853700022639857458120653987628066329553993455095744442603526286567635470262100697523310194662755680020791578323977842184628235173868186953730170666244481004716140892307428689543309319049255627698962764471254572167653360765237130434053515299989207086129141411866704091377809487032407585082685388521713825458321747461076256148245700446206524502555527895610747318156112364693242552815913866997387907039221878623550450440087976467924268698181025169316975079672023136147072866887726738750735785351995534738776787935962179264206558088507199532384760898767026665087389442214107266130381003634007223243753701794149247801616769273880406774243379702709774462912047338264418002296443441681246735900880928313326674254729844022168131266507821732893326329615422862061883651849364239357978527258545264669788706821213999409274526989966894476572112102837269672366895633648665768976454232257502896865362070028733582176958526424218512155876441970572987580055717119810735714447895521523729120183715435986142589459935321484978815212233463063377702925509621684566437399264106176081421656216320442316331283579359749487647768142358123901300844675837932919664058310515346725976731927828246774401460425795759341822690004244673536694761492395954067144962009334299144402637016162535393968439686977098894097694951265885483001684283474292812019024950968408209343595795373430537412972151439383716019568927873560567460996443620392124664770780465952412710840257660351956576465246037562083503058141237848302731013334640954233594918338047839783443627954620390165998757407154698561655693612790569227726781618570980367272910746557220189596804724048533864981198464974807614404840503822376155390276698480962584894640205493775474621700767127449405971823077928713219820813890772678292288672273095483757245339905849125679594768319428199441590076018545765708503691293221085110958494408954294863148916580859554743936117367650264255253411609673433656646549745667339402889190903337636045120738337485549734124095250537057893992743163921329566647484713018303247028516507778223120035175669306483773983632080889950216416302577903225018112483405544982084838681540203824932059186216839690556554217917063422962178301243224785532958069741274686866005367447961941893337015993047161204893603298727390644592746134059848844660490847885910355388086656259642910068207801722403773755150650762338474957739196949479761015378039050010833490346284011148140654404235390938354273852763878225148092864722391941135841759924515861415571777455156334872510331757380131319169774858077986806290508430140160783057981226955504871746562732730194641773127973223488970136154685160729872436431110108950840623779660222457676279974591436853345237788070101819572835355798594296642977795386081880749322356378475034740927229852443051932407790844676049227306901748175045045180780224309303787150138531924417558726458347174226569364397738468099097014729596945025622366217800276615433014992239767676747411663248509998913695915751200523253118476215529382000459945931995001154624034e1";
char * imstr= "-0.569549241894455861510720038697821794382097673806016675724332019937001216410137292907841100097231549234119392684326281714058471665061641202604631152886381163094006315143713962427984794198244894997311350759586228805917601014405964897823254977272840934041324840391725155969055856636053567560585268978993111767280573659109124291525304793730706194010979959903927246146204955228372267809123688464985035901371678602769337206502643469429330473311613889418778223554933388845334692611537371652853367708321986568225612509474177040539146752131367704405332193054363934335369946743145782950519404276662189453699898268292875369851208241744166128438446358527237308255505136106839516068014870240040407585740081466119419537714315747318256108106889887887136581489872091938541827315744465491170714217235158915310160915352274891618327680505275238702014325871882561518627630722580535743098352256288203535570893795175075340417992913432230190801257785102892902303943802684256179538693861794138504411254361143734384622298134032125202137990704546949211282784451673463115820932549452102536030937152007504689831660432802918676515360130436041937161888570958073813277949428937327094939862264215750146254973478840030494961261033234232307860693769298782691913662343999705765744770759968337306848473563815423369847085204793368457589088065223392338100015353419674836467282715542948555622400596582279327701542129544830116061757983225653435712086997831660897304319516314259172364096485288922189341790375741086178849062881990419049795839290460168222078400624559503634640631129510147351247213550547636929577640493564410830445517030393901828857233061096768235150317981751033432701459648978530571969570817541296650419634960309477208551512835834301592583719652018643456713810734945684905822136142303517656238040152172117136019362660511163690185104892161957209898419096346641417810359644156949353589620222328948013336303169041870610658428774030095947851960953830594256401426363749149397857119588791512206151871450885231255650753596268920724686600222380553356169933501678832976976776545617585017588241135812914964801358350352636556865382688654350372393449759980901398326020321316838161554714562664314940488126558276977175783957783537832805698118657004167477243130563653556318393497993513673065268113343700530813158320171874736699866508993240555014777720494140992989262699950764300578355556672410510443750690651135640994835484243940752671973596356361242171920913457376653925236223353986768048550806047405410346634308077139581808977326874637455363424184374355056128736318336630892866351728102008685378737570730094785679443352981425327318502683928663473190043639340989296195769228851782452846470888483543888610032473924230970093114970792707836649727018289032512988593032392617960133575233672059762578854824455467067005644662760490907826542243126612630956055475753608723396242559733010853981642396111585290512273164655654477749224933488288517966031477999828030113497019983016271046437969500548560046788751743659828403823622750818886102765259741243184118215790926275513830135963803290739577345347018546502631601244198006189029046708571256062039762553963605542500235926318166016041040555005767091607169304676972848163990065295442296998165853729349868293289104548599478930055932702801650168022634196051311305861528725978439133436425261304989507362753744219576409663657740587331320330785988357197921308234934635796426345155750506406685765033356144456796770231859799138291075728630166382727911396425664814445331861900474082348072421729063167054693872703293015732085683394948823358073439365829891878107548982721722749204536489612566132881534208702286211769370305141705416806038472919699632709881965288481025510559111791688323232364020565420418416891247612780033214452264173995207582247916854814313208838631386711895887414609977530036044372683479629131221481413916227213077126255176716274408301948585661088579892728165914448375679999791290610512221743209332513525553367921290718604617106224473275942721802769735638579094211676246277102703031424334347253527582201387811946874398950871554909881875499904780716888360694036677469934746575101917475982735863512948473315105132946370983995881871753801594774183941213581234045e0";
#endif /* HIPREC_FOR_0_POINT_3 */

/* =========================================================== */

int getmemusage (void)
{
	static int prev=0;
	system ("ps aux |grep hurwitz-euler | grep -v grep |cut -b 25-32 > /tmp/m");
	FILE * fh = fopen ("/tmp/m", "r");
	char buf[80];
	fgets (buf, 80, fh); 
	int mem = atoi (buf);
	fclose (fh);
	int delt = mem - prev;
	prev = mem;
	return delt;
}

/* =========================================================== */
	
void zeta_euler(cpx_t zeta, cpx_t ess, cpx_t q, int em, int prec);

void do_perf (int prec)
{
	int i;

	printf ("#\n# graph of performance of hurwitz zeta as function of precision \n#\n");
	int hz = sysconf (_SC_CLK_TCK);
	printf ("# clock ticks=%d\n#\n", hz);
	printf ("#\n# eval at s=0.5 +i 14.134725 and z=0.3 \n#\n");
	fflush (stdout);

	for (prec=10; prec <123123; prec *= 1.41)
	{
		/* Set the precision (number of binary bits) */
		int nbits = 3.3*prec+100;
		mpf_set_default_prec (nbits);

		printf ("%d\t%d\t", prec, nbits);

		cpx_t ess, zeta, zee, plog, expected;
		cpx_init (ess);
		cpx_init (zeta);
		cpx_init (zee);
		cpx_init (plog);
		cpx_init (expected);

		mpf_set_str (expected[0].re, restr, 10);
		mpf_set_str (expected[0].im, imstr, 10);
	
		mpf_t que;
		mpf_init (que);

		struct tms start, end;

		cpx_set_d (ess, 0.5, 14.134725);
		cpx_set_d (zee, 0.2, 0.0);
		cpx_set_d (zee, 0.3, 0.0);

#define BORWEIN_ALGO
#ifdef BORWEIN_ALGO
		/* First we warm the cache */
		getmemusage();
		times (&start);
		cpx_hurwitz_zeta (zeta, ess, que, prec);
		times (&end);
		int mem = getmemusage();
		printf ("duude memus=%d\n", mem);

		cpx_sub (zeta, zeta, expected);
		int rex = get_prec (zeta, prec);
		printf ("%d\t", rex);

		printf ("%jd\t", (intmax_t) (end.tms_utime - start.tms_utime));

		/* Then with a hot cache */
		times (&start);
		for (i=0; i<1000; i++)
			cpx_hurwitz_zeta (zeta, ess, que, prec);
		times (&end);

		intmax_t delta = end.tms_utime - start.tms_utime;
		// delta *= 10;
		printf ("%jd\t", delta);

		cpx_sub (zeta, zeta, expected);
		rex = get_prec (zeta, prec);
		printf ("%d\t", rex);

#endif

// #define TAYLOR_SERIES_ALGO
#ifdef TAYLOR_SERIES_ALGO
		/* First we warm the cache */
		times (&start);
		cpx_hurwitz_taylor (zeta, ess, zee, prec);
		times (&end);
		
		cpx_sub (zeta, zeta, expected);
		int rex = get_prec (zeta, prec);
		printf ("%d\t", rex);

		printf ("%jd\t", (intmax_t) (end.tms_utime - start.tms_utime));

		/* Then with a hot cache */
		times (&start);
		for (i=0; i<1000; i++)
			cpx_hurwitz_taylor (zeta, ess, zee, prec);
		times (&end);
		intmax_t delta = end.tms_utime - start.tms_utime;
		printf ("%jd\t", delta);
		
		cpx_sub (zeta, zeta, expected);
		rex = get_prec (zeta, prec);
		printf ("%d\t", rex);

#endif
		
// #define EULER_MACLAURIN_ALGO
#ifdef EULER_MACLAURIN_ALGO
		/* First we warm the cache */
		times (&start);
		cpx_hurwitz_euler (zeta, ess, zee, prec);
		times (&end);

		cpx_sub (zeta, zeta, expected);
		int rex = get_prec (zeta, prec);
		printf ("%d\t", rex);

		printf ("%jd\t", (intmax_t) (end.tms_utime - start.tms_utime));

		/* Then with a hot cache */
		times (&start);
		for (i=0; i<1000; i++)
			cpx_hurwitz_euler (zeta, ess, zee, prec);
		times (&end);
		printf ("%jd\t", (intmax_t) (end.tms_utime - start.tms_utime));

		cpx_sub (zeta, zeta, expected);
		rex = get_prec (zeta, prec);
		printf ("%d\t", rex);

#endif
		printf ("\n");

		cpx_clear (ess);
		cpx_clear (zeta);
		cpx_clear (zee);
		cpx_clear (plog);
		cpx_clear (expected);
		mpf_clear (que);

		fflush (stdout);
	}
}

/* Compute to high precision, monitor the precision */
void hiprec(cpx_t zeta, int prec)
{
	/* Set the precision (number of binary bits) */
	int nbits = 3.322*prec+2*3.14*3.32*prec+140;
	// nbits = 3.322*prec+ 7*prec + 140;
	nbits = 3.322*prec+ 140;
	mpf_set_default_prec (nbits);
	cpx_set_prec (zeta, nbits);

	cpx_t cq, ess, prevzeta;
	cpx_init (ess);
	cpx_init (cq);
	cpx_init (prevzeta);
	cpx_set (prevzeta, zeta);

	cpx_set_d (ess, 0.5, 14.134725);
	
	mpf_t que;
	mpf_init (que);
	mpf_set_d (que, 0.3);
	
	mpf_set (cq[0].re, que);
	mpf_set_ui (cq[0].im, 0);
	
	// cpx_hurwitz_zeta (zeta, ess, que, prec);
	// cpx_hurwitz_taylor (zeta, ess, cq, prec);
	cpx_hurwitz_euler (zeta, ess, cq, prec);
	cpx_sub (ess, zeta, prevzeta);

	printf ("prec=%d ", prec);

	long rex, imx;
	mpf_get_d_2exp (&rex, ess[0].re);
	mpf_get_d_2exp (&imx, ess[0].im);
	rex = -0.30103 *rex;
	imx = -0.30103 *imx;
	if (imx < rex) rex = imx;
	printf ("prev=%ld ", rex);

	double vre = mpf_get_d (ess[0].re);
	printf ("vre = %g ", vre);

#if 0
	double vim = mpf_get_d (ess[0].im);
	printf ("vim = %g ", vim);
#endif

#if 1
	// gmp_printf ("re= %.220Ff ", zeta[0].re);
	printf ("re= ");
	mpf_out_str (stdout, 10, prec, zeta[0].re);
	
	printf (" im= ");
	mpf_out_str (stdout, 10, prec, zeta[0].im);
#endif
	printf ("\n");
	fflush (stdout);

	cpx_clear (ess);
	cpx_clear (cq);
	cpx_clear (prevzeta);
}

double err_est (cpx_t ess, int em, int pee)
{
	/* (2pi)^{-2p} ((s+2p)! / (s-1)!)  (M+a)^{-2p}*/
	cpx_t poch;
	cpx_init (poch);

	cpx_poch_rising (poch, ess, 2*pee);

	mpf_t pabs, den;
	mpf_init (pabs);
	mpf_init (den);

	cpx_abs (pabs, poch);

	double fden = 2*3.1416*em;
	mpf_set_d (den, fden);
	fp_log (den, den, 40);
	mpf_mul_ui (den,den, 2*pee);
	mpf_neg (den,den);
	fp_exp(den,den,40);
	
	mpf_mul (pabs,pabs, den);

	double po = mpf_get_d (pabs);

	cpx_clear (poch);
	mpf_clear (pabs);
	mpf_clear (den);
	return po;
}

int main (int argc, char * argv[])
{
	int prec =50;

	if (argc < 2)
	{
		fprintf (stderr, "Usage: %s <prec>\n", argv[0]);
		exit (1);
	}
	prec = atoi(argv[1]);

	/* Set the precision (number of binary bits) */
	mpf_set_default_prec (3.322*prec+40);

	cpx_t ess, zeta, z2, z3;
	cpx_init (ess);
	cpx_init (zeta);
	cpx_init (z2);
	cpx_init (z3);

	cpx_set_d (ess, 0.5, 4.0);
	cpx_set_d (ess, 2.0, 0.1);
	cpx_set_d (ess, 0.5, 14.134725);
	
	mpf_t que;
	mpf_init (que);
	mpf_set_d (que, 0.2);
	
	cpx_t cq;
	cpx_init (cq);
	mpf_set (cq[0].re, que);
	mpf_set_ui (cq[0].im, 0);
	
#if 0
	int i;
	for (i=10; i < 10000; i*=1.5)
	// for (i=100; i > 10; i/=1.3)
	{
		hiprec (zeta, i);
	}
#endif
	
#if 0
	cpx_hurwitz_zeta (z2, ess, que, prec);
	fp_prt ("borwein ", z2[0].im);
	printf ("\n");

	cpx_hurwitz_taylor (z3, ess, cq, prec);
	fp_prt ("taylor  ", z3[0].im);
	printf ("\n");

	cpx_hurwitz_euler (zeta, ess, cq, prec);
	fp_prt ("euler   ", zeta[0].im);
	printf ("\n");
#endif

#if 0
	cpx_t expected;
	cpx_init (expected);
	mpf_set_str (expected[0].re, restr, 10);
	mpf_set_str (expected[0].im, imstr, 10);
	
	int em;
	for (i=0; i<500; i++)
	{
		em = prec + 12 -i;
		// double err = err_est (ess, em, pee);
		// printf ("err=%g ", err);
		zeta_euler (zeta, ess, cq, em, prec);
		cpx_sub (zeta, zeta, expected);

		int rex = get_prec (zeta, prec);
		
		printf ("m=%d ", em);
		printf ("prec=%d ", rex);
		// fp_prt ("eu-mac=", zeta[0].im);
		printf ("\n");
		fflush (stdout);
	}
#endif
	do_perf (prec);
	return 0;
}

