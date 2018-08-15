package testet;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.StringTokenizer;

//      125.128.37.86       that's my ipaddress

//   http://www.findip.kr/   real ip config

@SuppressWarnings("unused")

public class ToServer {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		String pre_step;
		// String pre_cal;
		DatagramSocket dsock1 = null;	//tizenID
		DatagramSocket dsock = null;
		DatagramSocket dsock2 = null;
		
		DatagramSocket ddsock1= null;
		DatagramSocket ddsock = null;
		DatagramSocket ddsock2 = null;
		

		String step = null;
		String step1 = null;
		// String calorie = null;

		Connection conn = null;

		Statement stmt = null;

		long time;

		String str;
		String str2;

		SimpleDateFormat dayTime = new SimpleDateFormat("yyyy-MM-dd hh:mm:ss.SSS");

		//String ID = null;

		try {

			Class.forName("com.mysql.jdbc.Driver");
			conn = DriverManager.getConnection("jdbc:mysql://117.16.123.55:3306/", "root", "root");
			System.out.println("Connected!!");
			stmt = conn.createStatement();
			stmt.executeUpdate("use hrdb;");

//			stmt.executeUpdate("drop table data");
//			stmt.executeUpdate(
//					"create table data(" + "ID varchar(50), time varchar(50), HR varchar(100), step varchar(100));");

		} catch (SQLException ex) {
			ex.printStackTrace();
		} catch (ClassNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		try {
			
			dsock1 = new DatagramSocket(4427);   //tizenID
			dsock = new DatagramSocket(4428);
			dsock2 = new DatagramSocket(4429);

			String spl1[];						//tizenID
			byte[] buf1 = new byte[256];
			DatagramPacket ID = new DatagramPacket(buf1, buf1.length);
			
			String spl[];
			byte[] buf = new byte[256];
			DatagramPacket hrm = new DatagramPacket(buf, buf.length);

			String spl2[];
			byte[] buf2 = new byte[256];
			DatagramPacket hrm2 = new DatagramPacket(buf2, buf2.length);
			System.out.println("Waitting...");
			
			ddsock1 = new DatagramSocket(4430);   //tizenID
			ddsock = new DatagramSocket(4431);
			ddsock2 = new DatagramSocket(4432);

			String sspl1[];						//tizenID
			byte[] bbuf1 = new byte[256];
			DatagramPacket IID = new DatagramPacket(bbuf1, bbuf1.length);
			
			String sspl[];
			byte[] bbuf = new byte[256];
			DatagramPacket hhrm = new DatagramPacket(bbuf, bbuf.length);

			String sspl2[];
			byte[] bbuf2 = new byte[256];
			DatagramPacket hhrm2 = new DatagramPacket(bbuf2, bbuf2.length);
			System.out.println("Waitting...");

			while (true) {
				// time value;
				time = System.currentTimeMillis();
				str = dayTime.format(new Date(time));

				// hrm
				dsock1.receive(ID);
				dsock.receive(hrm);
								
				// value insert

				String msg_ID = new String(ID.getData(), 0, ID.getLength());
				String msg_hrm = new String(hrm.getData(), 0, hrm.getLength());
				System.out.println("init ID : " + msg_ID + " msg_hrm : " + msg_hrm);
				dsock2.setSoTimeout(1000);
				try {
					dsock2.receive(hrm2);
				}

				// data not exist
				catch (Exception e) {
					StringTokenizer st = new StringTokenizer(msg_hrm, "|");
					String heart = st.nextToken();
					stmt.executeUpdate(
							"INSERT INTO data VALUES('" + msg_ID + "','" + str + "','" + heart + "','" + step + "');");
					System.out.println("tizenID : " + msg_ID + "\t" + "hrm : " + heart + "\t" + "step : " + step + "\t\n");

					continue;
				}

				// data exist
				String pedo = new String(hrm2.getData(), 0, hrm2.getLength());
				//StringTokenizer st_pedo = new StringTokenizer(pedo, "|");
				String temp = pedo;

				// pedo data token
				StringTokenizer st = new StringTokenizer(msg_hrm, "|");
				String heart = st.nextToken();
				step = temp;
				stmt.executeUpdate(
						"INSERT INTO data VALUES('" + msg_ID + "','" + str + "','" + heart + "','" + step + "');");
				System.out.println("\nID : " + msg_ID + "\t" + "hrm : " + heart + "\t" + "step : " + step + "\t\n");

				
				///////////////////////////////////////////////////////////////////////////////////////////////////
				
				//step
				ddsock1.receive(IID);
				ddsock.receive(hhrm);
				
				// value insert

				String msg_ID1 = new String(IID.getData(), 0, IID.getLength());
				String msg_hrm1 = new String(hhrm.getData(), 0, hhrm.getLength());
				System.out.println("init ID : " + msg_ID1 + " msg_hrm : " + msg_hrm1);
				ddsock2.setSoTimeout(1000);
				try {
					ddsock2.receive(hhrm2);
				}

				// data not exist
				catch (Exception e) {
					StringTokenizer st1 = new StringTokenizer(msg_hrm1, "|");
					String heart1 = st1.nextToken();
					stmt.executeUpdate(
							"INSERT INTO data VALUES('" + msg_ID1 + "','" + str + "','" + heart1 + "','" + step1 + "');");
					System.out.println("tizenID : " + msg_ID1 + "\t" + "hrm : " + heart1 + "\t" + "step : " + step1 + "\t\n");

					continue;
				}

				// data exist
				String pedo1 = new String(hhrm2.getData(), 0, hhrm2.getLength());
				//StringTokenizer st_pedo1 = new StringTokenizer(pedo1, "|");
				String temp1 = pedo1;

				// pedo data token
				StringTokenizer st1 = new StringTokenizer(msg_hrm1, "|");
				String heart1 = st1.nextToken();
				step1 = temp1;
				stmt.executeUpdate(
						"INSERT INTO data VALUES('" + msg_ID1 + "','" + str + "','" + heart1 + "','" + step1 + "');");
				System.out.println("\nID : " + msg_ID1 + "\t" + "hrm : " + heart1 + "\t" + "step : " + step1 + "\t\n");					
				
			}

		} catch (Exception e) {
			e.printStackTrace();

		} finally {

			if (dsock != null)

				dsock.close();

		}

	}

}