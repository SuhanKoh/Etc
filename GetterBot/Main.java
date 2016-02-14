import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.ProtocolException;
import java.net.URL;

public class Main {

    public static final String USER_AGENT = "Mozilla/5.0";
    public static final int WAIT_TIME = 1500000;

    public static final String URL = "YOUR URL HERE";

    public static void main(String[] args) {
        // write your code here

        while (true) {

            try {
                URL obj = new URL(URL);
                HttpURLConnection connection = (HttpURLConnection) obj.openConnection();

                connection.setRequestMethod("GET");
                connection.setRequestProperty("User-Agent", USER_AGENT);

                BufferedReader in = new BufferedReader(new InputStreamReader(connection.getInputStream()));

                String inputLine;

                System.out.println("");

                while ((inputLine = in.readLine()) != null) {
                    System.out.println(inputLine);
                }
                in.close();

                System.out.println("Response Code : " + connection.getResponseCode());

                Thread.sleep(WAIT_TIME);

            } catch (InterruptedException | IOException e) {
                e.printStackTrace();
                break;
            }
        }
    }
}
