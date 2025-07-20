/*

    DA ScriVERe

*/
// -----------------------------------------------------------------------------
// Compilazione:
//     java ClientThreadREST numeri-primi 1 50
// -----------------------------------------------------------------------------

import java.io.*;
import java.net.*;
import java.util.*;
import java.util.regex.*;

public class ClientThreadREST {
    public static void main(String[] args) {
        if (args.length < 3) {
            System.out.println("USO: java ClientThreadREST <calcola-somma|numeri-primi> op1 op2");
            return;
        }

        final String servizio = args[0];
        final String op1 = args[1];
        final String op2 = args[2];

        // Avviamo 3 thread concorrenti
        for (int i = 0; i < 3; ++i) {
            Thread t = new Thread(new Worker("127.0.0.1", servizio, op1, op2));
            t.setName("T" + (i + 1));
            t.start();
        }
    }
}

/***************************** WORKER *****************************/
class Worker implements Runnable {
    private final String server;
    private final String service;
    private final String param1;
    private final String param2;

    Worker(String server, String service, String p1, String p2) {
        this.server = server;
        this.service = service;
        this.param1 = p1;
        this.param2 = p2;
    }

    @Override
    public void run() {
        switch (service) {
            case "calcola-somma":
                float a = Float.parseFloat(param1);
                float b = Float.parseFloat(param2);
                float somma = calcolaSomma(a, b);
                System.out.printf("[%s] Somma = %.2f\n", Thread.currentThread().getName(), somma);
                break;

            case "numeri-primi":
                int x = Integer.parseInt(param1);
                int y = Integer.parseInt(param2);
                PrimeResult res = primeNumber(x, y);
                System.out.printf("[%s] Primi (%d): %s\n", Thread.currentThread().getName(),
                                  res.count, res.primes);
                break;

            default:
                System.err.println("Servizio non riconosciuto: " + service);
        }
    }

    /****************** CHIAMATE REST ******************/

    private float calcolaSomma(float v1, float v2) {
        String url = String.format("http://%s:8000/calcola-somma?param1=%f&param2=%f", server, v1, v2);
        String json = httpGET(url);
        if (json == null) return -1.0f;

        Matcher m = Pattern.compile("\\\"somma\\\"\\s*:\\s*([0-9.+-Ee]+)").matcher(json);
        return m.find() ? Float.parseFloat(m.group(1)) : -1.0f;
    }

    private PrimeResult primeNumber(int a, int b) {
        String url = String.format("http://%s:8000/numeri-primi?param1=%d&param2=%d", server, a, b);
        String json = httpGET(url);
        if (json == null) return PrimeResult.EMPTY;

        // Estraiamo il conteggio
        int count = 0;
        Matcher mCnt = Pattern.compile("\\\"totale_numeri_primi\\\"\\s*:\\s*(\\d+)").matcher(json);
        if (mCnt.find()) count = Integer.parseInt(mCnt.group(1));

        // Estraiamo la lista
        List<Integer> list = new ArrayList<>();
        Matcher mArr = Pattern.compile("\\\"primi\\\"\\s*:\\s*\\[(.*?)\\]").matcher(json);
        if (mArr.find()) {
            String body = mArr.group(1).trim();
            if (!body.isEmpty()) {
                for (String tok : body.split(",")) list.add(Integer.parseInt(tok.trim()));
            }
        }
        return new PrimeResult(count, list);
    }

    /******************* HTTP GET RAW *******************/
    private String httpGET(String urlString) {
        try {
            URL url = new URL(urlString);
            URLConnection conn = url.openConnection();
            try (BufferedReader br = new BufferedReader(new InputStreamReader(conn.getInputStream()))) {
                StringBuilder sb = new StringBuilder();
                String line;
                while ((line = br.readLine()) != null) sb.append(line);
                return sb.toString();
            }
        } catch (IOException ex) {
            System.err.println("[" + Thread.currentThread().getName() + "] I/O error: " + ex.getMessage());
        }
        return null;
    }
}

/************************* DTO RISULTATO *************************/
class PrimeResult {
    static final PrimeResult EMPTY = new PrimeResult(0, Collections.emptyList());

    final int count;
    final List<Integer> primes;

    PrimeResult(int count, List<Integer> primes) {
        this.count = count;
        this.primes = primes;
    }

    @Override
    public String toString() {
        return primes.toString();
    }
}