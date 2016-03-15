import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

public class SatelliteTracker extends JPanel 
{
	   private static final int RECT_W = 360*4+20;
	   private static final int RECT_H = 180*5+50;
	   private static SatCalculator sc = null;
	   
	   public static void main(String[] args) 
	   {
		   sc = new SatCalculator();
		   
		   SwingUtilities.invokeLater(new Runnable() 
		   {
		         public void run() 
		         {
		            createAndShowGui();
		         }
		      });
	   }
	   
	    public void paint(Graphics g) 
	    {
	    	drawGrid(g);
	    	plotPath(g);
	    }	   

	   private static void createAndShowGui() 
	   {
			SatelliteTracker mainPanel = new SatelliteTracker();
			JFrame frame = new JFrame("Satellite Tracker");
			//JLabel background=new JLabel(new ImageIcon("F:\\acrylics1.png"));
			// frame.add(background);
			// frame.setContentPane(new JLabel(new ImageIcon("F:\\acrylics1.png")));
			frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
			frame.getContentPane().add(mainPanel);
			frame.pack();
			frame.setLocationByPlatform(true);
			frame.setSize(RECT_W, RECT_H);
			frame.setVisible(true);
	   }
	   
	   private static void drawGrid(Graphics g)
	   {
		   for (int lat = -90; lat <= 90; lat+= 10)
		   {
			   g.drawLine(0, (lat + 90)*5, RECT_W, (lat + 90)*5);
			   
			   if (lat == 0)
			   {
				   g.drawLine(0, (lat + 90)*5-1, RECT_W, (lat + 90)*5-1);
				   g.drawLine(0, (lat + 90)*5+1, RECT_W, (lat + 90)*5+1);
				   
			   }
		   }
		   for (int lon = -180; lon <= 180; lon+= 10)
		   {
			   g.drawLine((lon + 180)*4, 0, (lon + 180)*4, RECT_H);
		   }
		   
		   g.drawString("Inclination: " + sc.getInclination(), 10, 20);
		   g.drawString("RAAN: " + sc.getRaan(), 10, 40);
		   g.drawString("Decimal GMST: " + sc.getDecimalGmst(), 10, 60);
		   g.drawString("Ascension Node: " + sc.getAscensionNode(), 10, 80);
	   }
	   
	   private static void plotPath(Graphics g)
	   {
		   for (int lon = -180; lon < 180; lon+= 1)
		   {
			   g.drawLine((lon + 180)*4, 
					   (int)(Math.sin(lon*2.0*Math.PI/360.0)*sc.getInclination()*5 + 90*5), 
					   (lon + 181)*4, 
					   (int)(Math.sin((lon+1)*2.0*Math.PI/360.0)*sc.getInclination()*5 + 90*5));
			   // g.fillOval((lon + 180)*4, (int)(Math.sin(lon*2.0*Math.PI/360.0)*sc.getInclination()*5 + 90*5), 10, 10);
		   }
	   }
	   

}