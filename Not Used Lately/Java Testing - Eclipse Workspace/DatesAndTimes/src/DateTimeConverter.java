import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.TimeZone;

// Program that converts current solar date/time to Julian
// and sidereal
public class DateTimeConverter 
{
	private static Date date;
	private static String[] ISS_KEPS = {	"ISS",
										"1 25544U 98067A   16063.85205029  .00007024  00000-0  11228-3 0  9997",
										"2 25544  51.6424 223.3270 0003333 190.1315 281.8711 15.54368576988592"};
	
	public static void main(String[] args) 
	{

		TimeZone tz  = TimeZone.getDefault() ;
		TimeZone.setDefault(TimeZone.getTimeZone("UTC"));		
		Calendar cal = Calendar.getInstance();
		// Calendar cal = Calendar.getInstance(TimeZone.getTimeZone("UTC"));
		System.out.println(cal.getTime());
		System.out.println((cal.get(Calendar.MONTH)+1) + "/" +
							cal.get(Calendar.DAY_OF_MONTH) + " " +
							cal.get(Calendar.HOUR_OF_DAY) + ":" + 
							cal.get(Calendar.MINUTE) + ":" +
							cal.get(Calendar.SECOND));
		//System.out.println(Calendar.HOUR + ":" + Calendar.MINUTE + ":" + Calendar.SECOND);
		double decimalGmst = getDecimalGmst(cal);
		findSat(ISS_KEPS, decimalGmst);
	}

	private static void findSat(String[] keps, double decimalGmst)
	{
		String kepsLine1 = keps[0];
		String kepsLine2 = keps[1];
		String kepsLine3 = keps[2];
		String[] line2elements = kepsLine2.split(" ");
		String[] line3elements = kepsLine3.split(" ");
		
		System.out.println("\n-------------------------------------------------------");
		System.out.println("Searching for " + kepsLine1);
		System.out.println("\n" + kepsLine2);
		System.out.println(kepsLine3 + "\n");
		
		System.out.println("Inclination: " + line3elements[3]);
		System.out.println("RAAN: " + line3elements[4]);
	}
	
	//@SuppressWarnings("deprecation")
	private static double getDecimalGmst(Calendar cal)
	{
	  int a = (14 - (cal.get(Calendar.MONTH) + 1)) / 12;
	  int y = cal.get(Calendar.YEAR) + 4800 - a;
	  int m = cal.get(Calendar.MONTH) + 1 + (12 * a) - 3;
	  double jdn = cal.get(Calendar.DAY_OF_MONTH) + (153*m + 2)/5 + (365L*(long)y) + y/4 - y/100 + y/400 - 32045;
	  double jd = (double)jdn + 
			  		(((double)cal.get(Calendar.HOUR_OF_DAY))-12.0)/24.0 + 
			  		(double)cal.get(Calendar.MINUTE)/1440.0 + 
			  		(double)cal.get(Calendar.SECOND)/86400.0;
	  /*
	  double jd0 = (double)jdn + 
			  		(((double)cal.get(0))-12.0)/24.0 + 
			  		(double)cal.get(0)/1440.0 + 
			  		(double)cal.get(0)/86400.0;
	  
	   */
	  double UT = cal.get(Calendar.HOUR_OF_DAY) + cal.get(Calendar.MINUTE)/60.0 + cal.get(Calendar.SECOND)/3600.0;
	  double jd0 = jd - (UT/24.0);
	  
	  double D = jd - 2451545.0;
	  double D0 = jd0 - 2451545.0;
	  double GMST = 18.697374558 + 24.06570982441908 * D;
	  double T2 = D/36525.0;
	  double GMST2 =  6.697374558 + 0.06570982441908*D0 + 1.00273790935*cal.get(Calendar.HOUR_OF_DAY)  + 0.000026*T2*T2;
	  double T = (jd0-245145.0)/36525.0;
	  double T0 = 6.697374558+ (2400.051336*T)+(0.000025862*T*T)+(UT*1.0027379093);
	  /*
	  System.out.println("T0: "+ T0);
	  double T0normdec = T0/24.0;
	  System.out.println("T0normdec: " + T0normdec);
	  double T0left = ((int)T0normdec) * 1.0;
	  System.out.println("T0left: " + T0left);
	  double T0right = T0normdec - T0left;
	  System.out.println("T0right: " + T0right);
	  System.out.println("T0 norm: " + T0right * 24.0);
	  */
	  
	  while (T0 >= 24.0)
		  T0 -= 24.0;
	  
	  System.out.println("Normalized T0: " + T0);
	  
	  
	  	  
	  System.out.println("Julian Date: " + jd);
	  System.out.println("JD0: " + jd0);
	  System.out.println("UT (decimal hrs): " + UT);
	  System.out.println("GMST: " + GMST);
	  System.out.println("GMST2: " + GMST2);
	  
	  
	  //while (GMST >= 360.0)
		  //GMST -= 360.0;

	  while (GMST >= 24.0)
		  GMST -= 24.0;
	
	  System.out.println("GMST hrs = " + GMST);
	  int GMSTh = (int)GMST;
	  double GMSTright = GMST - (double)GMSTh;
	  System.out.println("GMSTright: " + GMSTright);
	  double GMSTm = 60.0*GMSTright;
	  System.out.println("Decimal minutes: " + GMSTm);
	  double mright = GMSTm - (int)GMSTm;
	  System.out.println("mright: " + mright);
	  double GMSTs = mright * 60.0;
	  System.out.println("GMST: " + (int)GMST + ":" + (int)GMSTm + ":" + (int)GMSTs);
	  
	  
	  while (GMST2 >= 24.0)
		  GMST2 -= 24.0;
		  
	  System.out.println("GMST2 hrs = " + GMST2);
	  //double T=(jd-2451545.0)/36525.0;
	  //double T0 = 6.697374558+ (2400.051336*T)+(0.000025862*T2)+(UT*1.0027379093);
	  /*
	  System.out.println("a="+a);
	  System.out.println("y="+y);
	  System.out.println("m="+m);
	  System.out.println((((double)cal.get(Calendar.HOUR_OF_DAY))-12.0)/24.0); 
	  System.out.println(7.0/24.0);
	  System.out.println(35.0/1440.0);
	  */
	  return GMST;
	}
}
