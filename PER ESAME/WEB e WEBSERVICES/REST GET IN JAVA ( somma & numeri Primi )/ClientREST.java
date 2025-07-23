/*

    SU WIRESHARK applico questo filtro: http.request.method == "GET" && tcp.port == 8000

    Posso lanciare:
        - java ClientREST numeri-primi 0 30
        - java ClientREST calcola-somma 10 30

*/

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class ClientREST {

    public static void main(String[] args) {
        if (args.length < 3) {
            System.out.println("USAGE: java ClientREST <calcola-somma|numeri-primi> op1 op2");
            return;
        }

        RESTAPI api = new RESTAPI("127.0.0.1");

        switch (args[0]) {
            case "calcola-somma": {
                float a = Float.parseFloat(args[1]);
                float b = Float.parseFloat(args[2]);
                float somma = api.calcolaSomma(a, b);
                System.out.printf("Somma = %.2f\n", somma);
                break;
            }
            case "numeri-primi": {
                int a = Integer.parseInt(args[1]);
                int b = Integer.parseInt(args[2]);
                PrimeResult result = api.primeNumber(a, b);
                if (result != null) {
                    System.out.println("Primi trovati (" + result.count + ") : " + result.primes);
                }
                break;
            }
            default:
                System.out.println("Servizio non riconosciuto!");
        }
    }
}

/**
 * Contenitore per il risultato di primeNumber():
 *   – count  : quanti numeri primi sono stati trovati
 *   – primes : lista completa degli interi primi
 */
class PrimeResult {
    final int count;
    final List<Integer> primes;

    PrimeResult(int count, List<Integer> primes) {
        this.count = count;
        this.primes = primes;
    }
}

/**
 * Classe helper che incapsula le chiamate HTTP/REST.
 */
class RESTAPI {
    private final String server;

    RESTAPI(String remoteServer) {
        server = remoteServer;
    }

    /**
     * Chiama /calcola-somma e restituisce la somma dei due float.
     */
    float calcolaSomma(float val1, float val2) {
        String url = "http://" + server + ":8000/calcola-somma?param1=" + val1 + "&param2=" + val2;
        String risposta = httpGET(url);

        // Estraggo la chiave "somma" dal JSON { "somma": <numero> }
        Pattern p = Pattern.compile("\"somma\"\\s*:\\s*([0-9.+-Ee]+)");
        Matcher m = p.matcher(risposta);
        if (m.find()) {
            return Float.parseFloat(m.group(1));
        }
        System.err.println("[CLIENT] JSON inatteso: " + risposta);
        return -1.0f;
    }

    /**
     * Chiama /numeri-primi e restituisce sia il conteggio che la lista completa.
     */
    PrimeResult primeNumber(int a, int b) {
        String url = "http://" + server + ":8000/numeri-primi?param1=" + a + "&param2=" + b;
        String risposta = httpGET(url);

        if (risposta == null) return null;

        int count = 0;
        List<Integer> lista = new ArrayList<>();

        // 1) Estrae il conteggio totale
        Pattern pCnt = Pattern.compile("\"totale_numeri_primi\"\\s*:\\s*(\\d+)");
        Matcher mCnt = pCnt.matcher(risposta);
        if (mCnt.find()) {
            count = Integer.parseInt(mCnt.group(1));
        }

        // 2) Estrae l'array [ ... ]
        Pattern pArr = Pattern.compile("\"primi\"\\s*:\\s*\\[(.*?)\\]");
        Matcher mArr = pArr.matcher(risposta);
        if (mArr.find()) {
            String arrayBody = mArr.group(1).trim();
            if (!arrayBody.isEmpty()) {
                for (String token : arrayBody.split(",")) {
                    lista.add(Integer.parseInt(token.trim()));
                }
            }
        }

        if (lista.size() != count) {
            System.err.println("[CLIENT] Avviso: mismatch conteggio/lista nel JSON!");
        }

        return new PrimeResult(count, lista);
    }

    /*********** Helper di basso livello per la GET HTTP ***********/
    private String httpGET(String urlString) {
        try {
            URL url = new URL(urlString);
            URLConnection conn = url.openConnection();
            conn.connect();
            try (BufferedReader br = new BufferedReader(new InputStreamReader(conn.getInputStream()))) {
                StringBuilder sb = new StringBuilder();
                String line;
                while ((line = br.readLine()) != null) {
                    sb.append(line);
                }
                return sb.toString();
            }
        } catch (MalformedURLException e) {
            System.err.println("URL errato: " + urlString);
        } catch (IOException e) {
            System.err.println("Errore I/O: " + e.getMessage());
        }
        return null;
    }
}