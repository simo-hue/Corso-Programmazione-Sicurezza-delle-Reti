import java.io.*; 
import java.net.*;
import java.util.*;

class ClientThreadREST {
    public static void main(String args[]) {
        if (args.length < 3) {
            System.out.println("USAGE: java ClientThreadREST numeri-primi min max");
        } else {
            int min = Integer.parseInt(args[1]);
            int max = Integer.parseInt(args[2]);
            int intervallo = (max - min + 1) / 3;

            RESTAPI t1 = new RESTAPI("127.0.0.1", 8001, args[0], min, min + intervallo - 1);
            RESTAPI t2 = new RESTAPI("127.0.0.1", 8002, args[0], min + intervallo, min + 2 * intervallo - 1);
            RESTAPI t3 = new RESTAPI("127.0.0.1", 8003, args[0], min + 2 * intervallo, max);

            t1.start();
            t2.start();
            t3.start();

            try {
                t1.join(); t2.join(); t3.join();

                // Unione dei risultati
                List<Integer> tuttiPrimi = new ArrayList<>();
                tuttiPrimi.addAll(t1.listaPrimi);
                tuttiPrimi.addAll(t2.listaPrimi);
                tuttiPrimi.addAll(t3.listaPrimi);

                Collections.sort(tuttiPrimi);

                System.out.println("Numeri primi trovati: " + tuttiPrimi.size());
                System.out.println("Lista completa: " + tuttiPrimi);
            } catch (InterruptedException e) {
                System.out.println("Errore nei thread: " + e.getMessage());
            }
        }
    }
}

class RESTAPI extends Thread {
    String server, service;
    int port, min, max;
    List<Integer> listaPrimi = new ArrayList<>();

    RESTAPI(String server, int port, String service, int min, int max) {
        this.server = server;
        this.port = port;
        this.service = service;
        this.min = min;
        this.max = max;
    }

    public void run() {
        if (!service.equals("numeri-primi")) {
            System.out.println("Servizio non disponibile!");
            return;
        }

        try {
            String urlStr = "http://" + server + ":" + port + "/numeri-primi?min=" + min + "&max=" + max;
            URL url = new URL(urlStr);
            URLConnection conn = url.openConnection();
            conn.connect();

            BufferedReader in = new BufferedReader(new InputStreamReader(conn.getInputStream()));
            String line;
            StringBuilder jsonBuilder = new StringBuilder();

            while ((line = in.readLine()) != null) {
                jsonBuilder.append(line);
            }

            parseJSON(jsonBuilder.toString());
        } catch (Exception e) {
            System.out.println("Errore: " + e.getMessage());
        }
    }

    void parseJSON(String json) {
        int iStart = json.indexOf("[");
        int iEnd = json.indexOf("]");

        if (iStart == -1 || iEnd == -1) return;

        String lista = json.substring(iStart + 1, iEnd);
        String[] numeri = lista.split(",");

        for (String s : numeri) {
            try {
                listaPrimi.add(Integer.parseInt(s.trim()));
            } catch (NumberFormatException e) {
                // Ignora
            }
        }
    }
}