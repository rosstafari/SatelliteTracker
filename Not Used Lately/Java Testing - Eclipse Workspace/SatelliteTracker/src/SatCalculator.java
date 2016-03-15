import java.util.Calendar;
import java.util.Date;
import java.util.TimeZone;

public class SatCalculator 
{
	private Date date;
	private String[] ISS_KEPS = {	"ISS",
										"1 25544U 98067A   16063.85205029  .00007024  00000-0  11228-3 0  9997",
										"2 25544  51.6424 223.3270 0003333 190.1315 281.8711 15.54368576988592"};
	private double inclination;
	private double raan;
	private double ascensionNode;
	private double decimalGmst;
	
	public SatCalculator()
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
		decimalGmst = getDecimalGmst(cal);

		
		
		
		findSat(ISS_KEPS, decimalGmst);
		double ariesLon = decimalGmst * 360.0 / 24.0;
		
		if (ariesLon > 180.0)
			ariesLon = 360.0-ariesLon;
		else
			ariesLon *= -1.0;
			
		System.out.println("Aries longitude: " + ariesLon);
		System.out.println("RAAN: " + raan);
		ascensionNode = raan + ariesLon;  // TODO: Fix for all times
		System.out.println("RAAN + Aries: " + ascensionNode);
		//double raanLon = raan;
	//	if (raanLon > 180)
		//	raanLon
	}
	
	public void findSat(String[] keps, double decimalGmst)
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
		inclination = Double.parseDouble(line3elements[3]);
		System.out.println("RAAN: " + line3elements[4]);
		raan = Double.parseDouble(line3elements[4]);
	}
	
	//@SuppressWarnings("deprecation")
	public double getDecimalGmst(Calendar cal)
	{
	  int a = (14 - (cal.get(Calendar.MONTH) + 1)) / 12;
	  int y = cal.get(Calendar.YEAR) + 4800 - a;
	  int m = cal.get(Calendar.MONTH) + 1 + (12 * a) - 3;
	  System.out.println(a + " " + y + " " + m);
	  double jdn = cal.get(Calendar.DAY_OF_MONTH) + (153*m + 2)/5 + (365L*(long)y) + y/4 - y/100 + y/400 - 32045;
	  double jd = (double)jdn + 
			  		(((double)cal.get(Calendar.HOUR_OF_DAY))-12.0)/24.0 + 
			  		(double)cal.get(Calendar.MINUTE)/1440.0 + 
			  		(double)cal.get(Calendar.SECOND)/86400.0;
	  
	  System.out.println((((double)cal.get(Calendar.HOUR_OF_DAY))-12.0)/24.0);
	  System.out.println((double)cal.get(Calendar.MINUTE)/1440.0);
	  System.out.println((double)cal.get(Calendar.SECOND)/86400.0);

	  System.out.println("jdn: " + jdn);
	  System.out.println("jd: " + jd);
	  double D = jd - 2451545.0;
	  System.out.println("D: " + D);
	  double GMST = 18.697374558 + 24.06570982441908 * D;

	  System.out.print("D*#: "); 
	  System.out.println( 24.06570982441908 * D);
	  System.out.println("Raw GMST: " + GMST);

	  // tmp
	  double gleft = (int)GMST * 1.0;
	  double gright = GMST - gleft;
	  System.out.println("gleft: " + gleft);
	  System.out.println("gright: " + gright);
	  System.out.println("mod: " + (int)gleft % 24);
	  
	  while (GMST >= 24.0)
		  GMST -= 24.0;	  
	  

	  System.out.println("decimal gmst = " + GMST);
	  
	  
	  int GMSTh = (int)GMST;
	  double GMSTright = GMST - (double)GMSTh;
	  double GMSTm = 60.0*GMSTright;
	  double mright = GMSTm - (int)GMSTm;
	  double GMSTs = mright * 60.0;
	  System.out.println("GMST: " + (int)GMST + ":" + (int)GMSTm + ":" + (int)GMSTs);
	  

	  return GMST;
	}

	public double getInclination() {
		return inclination;
	}
	public double getRaan() {
		return raan;
	}
	public double getDecimalGmst() {
		return decimalGmst;
	}
	public double getAscensionNode() {
		return this.ascensionNode;
	}
}

