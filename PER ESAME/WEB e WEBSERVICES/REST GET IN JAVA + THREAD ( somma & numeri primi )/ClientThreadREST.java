/*

    SU WIRESHARK applico questo filtro: http.request.method == "GET" && tcp.port >= 8000

    Posso lanciare:
        - java ClientThreadREST numeri-primi 0 30
        - java ClientThreadREST calcola-somma 10 30


    time java ClientThreadREST numeri-primi 1 100000000  -> 5.497 total
            rispetto a 
    time java ClientREST numeri-primi 1 100000000 -> 55.552 total
    
*/

import java.io.*;
import java.net.*;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.Collections;

class ClientThreadREST {
    private static final String SERVER = "127.0.0.1";
    private static final int[] PORTS = {8000, 8001, 8002}; // 3 server su porte diverse
    private static final int NUM_THREADS = 3;
    
    public static void main(String args[]) {
        if(args.length < 3) {
            System.out.println("USAGE: java ClientThreadREST <calcola-somma|numeri-primi> op1 op2");
            System.out.println("Esempi:");
            System.out.println("  java ClientThreadREST calcola-somma 10 30");
            System.out.println("  java ClientThreadREST numeri-primi 0 100");
            System.out.println("\nNOTA: Per numeri-primi, assicurati di avere 3 server attivi su porte 8000, 8001, 8002");
            return;
        }
        
        String service = args[0];
        String param1 = args[1];
        String param2 = args[2];
        
        if (service.equals("calcola-somma")) {
            // Per la somma, usa il primo server disponibile
            RESTAPI api = new RESTAPI(SERVER, PORTS[0]);
            float a = Float.parseFloat(param1);
            float b = Float.parseFloat(param2);
            float somma = api.calcolaSomma(a, b);
            System.out.printf("Somma = %.2f\n", somma);
            
        } else if (service.equals("numeri-primi")) {
            // Per i numeri primi, usa 3 server paralleli
            int start = Integer.parseInt(param1);
            int end = Integer.parseInt(param2);
            
            if (start > end) {
                System.err.println("Errore: l'intervallo non è valido (start > end)");
                return;
            }
            
            System.out.println("Calcolo numeri primi da " + start + " a " + end + " con " + NUM_THREADS + " server paralleli...");
            System.out.println("Server utilizzati: localhost:" + PORTS[0] + ", localhost:" + PORTS[1] + ", localhost:" + PORTS[2]);
            
            long startTime = System.currentTimeMillis();
            PrimeResult result = calcolaNumeriPrimiParallelo(start, end);
            long endTime = System.currentTimeMillis();
            
            if (result != null) {
                System.out.println("Primi trovati (" + result.count + "): " + result.primes);
                System.out.println("Tempo totale: " + (endTime - startTime) + "ms");
            } else {
                System.err.println("Errore nel calcolo dei numeri primi");
            }
            
        } else {
            System.out.println("Servizio '" + service + "' non riconosciuto!");
        }
    }
    
    private static PrimeResult calcolaNumeriPrimiParallelo(int start, int end) {
        ExecutorService executor = Executors.newFixedThreadPool(NUM_THREADS);
        List<Future<PrimeResult>> futures = new ArrayList<>();
        
        // Dividi il range in 3 parti
        int rangeSize = end - start + 1;
        int chunkSize = rangeSize / NUM_THREADS;
        
        for (int i = 0; i < NUM_THREADS; i++) {
            final int threadStart = start + (i * chunkSize);
            final int threadEnd = (i == NUM_THREADS - 1) ? end : threadStart + chunkSize - 1;
            final int threadId = i + 1;
            final int serverPort = PORTS[i]; // Ogni thread usa una porta diversa
            
            System.out.println("Thread " + threadId + ": range [" + threadStart + ", " + threadEnd + "] → server port " + serverPort);
            
            Future<PrimeResult> future = executor.submit(() -> {
                RESTAPI api = new RESTAPI(SERVER, serverPort);
                return api.calcolaNumeriPrimi(threadStart, threadEnd, threadId);
            });
            futures.add(future);
        }
        
        // Raccogli e unisci i risultati
        List<Integer> allPrimes = new ArrayList<>();
        int totalCount = 0;
        
        for (int i = 0; i < futures.size(); i++) {
            try {
                PrimeResult partialResult = futures.get(i).get(15, TimeUnit.SECONDS);
                if (partialResult != null) {
                    allPrimes.addAll(partialResult.primes);
                    totalCount += partialResult.count;
                    System.out.println("Thread " + (i+1) + " completato: " + partialResult.count + " primi trovati");
                } else {
                    System.err.println("Thread " + (i+1) + " ha restituito null");
                }
            } catch (TimeoutException e) {
                System.err.println("Thread " + (i+1) + " timeout dopo 15 secondi");
            } catch (Exception e) {
                System.err.println("Thread " + (i+1) + " errore: " + e.getMessage());
            }
        }
        
        executor.shutdown();
        
        if (allPrimes.isEmpty()) {
            return null;
        }
        
        // Ordina il risultato finale
        Collections.sort(allPrimes);
        
        return new PrimeResult(totalCount, allPrimes);
    }
}

class RESTAPI {
    private final String server;
    private final int port;
    private static final int CONNECTION_TIMEOUT = 2000; // 2 secondi
    private static final int READ_TIMEOUT = 10000;      // 10 secondi per calcoli lunghi
    
    public RESTAPI(String server, int port) {
        this.server = server;
        this.port = port;
    }
    
    public float calcolaSomma(float val1, float val2) {
        String url = "http://" + server + ":" + port + "/calcola-somma?param1=" + val1 + "&param2=" + val2;
        String response = httpGET(url, "calcola-somma");
        
        if (response == null) return -1.0f;
        
        Pattern pattern = Pattern.compile("\"somma\"\\s*:\\s*([0-9.+-Ee]+)");
        Matcher matcher = pattern.matcher(response);
        if (matcher.find()) {
            return Float.parseFloat(matcher.group(1));
        }
        
        System.err.println("JSON inatteso: " + response);
        return -1.0f;
    }
    
    public PrimeResult calcolaNumeriPrimi(int start, int end, int threadId) {
        String url = "http://" + server + ":" + port + "/numeri-primi?param1=" + start + "&param2=" + end;
        String response = httpGET(url, "Thread " + threadId + " (port " + port + ")");
        
        if (response == null) return null;
        
        int count = 0;
        List<Integer> primes = new ArrayList<>();
        
        // Estrae il conteggio
        Pattern countPattern = Pattern.compile("\"count\"\\s*:\\s*(\\d+)");
        Matcher countMatcher = countPattern.matcher(response);
        if (countMatcher.find()) {
            count = Integer.parseInt(countMatcher.group(1));
        }
        
        // Estrae l'array dei numeri primi
        Pattern arrayPattern = Pattern.compile("\"primi\"\\s*:\\s*\\[(.*?)\\]");
        Matcher arrayMatcher = arrayPattern.matcher(response);
        if (arrayMatcher.find()) {
            String arrayContent = arrayMatcher.group(1).trim();
            if (!arrayContent.isEmpty()) {
                for (String num : arrayContent.split(",")) {
                    primes.add(Integer.parseInt(num.trim()));
                }
            }
        }
        
        return new PrimeResult(count, primes);
    }
    
    private String httpGET(String urlString, String context) {
        HttpURLConnection conn = null;
        try {
            URL url = new URL(urlString);
            conn = (HttpURLConnection) url.openConnection();
            
            conn.setConnectTimeout(CONNECTION_TIMEOUT);
            conn.setReadTimeout(READ_TIMEOUT);
            conn.setRequestMethod("GET");
            conn.setUseCaches(false);
            conn.setRequestProperty("Connection", "close");
            
            int responseCode = conn.getResponseCode();
            if (responseCode != 200) {
                System.err.println("[" + context + "] HTTP Error: " + responseCode);
                return null;
            }
            
            try (BufferedReader reader = new BufferedReader(
                    new InputStreamReader(conn.getInputStream(), "UTF-8"))) {
                StringBuilder response = new StringBuilder();
                String line;
                while ((line = reader.readLine()) != null) {
                    response.append(line);
                }
                return response.toString();
            }
            
        } catch (SocketTimeoutException e) {
            System.err.println("[" + context + "] Timeout: " + e.getMessage());
        } catch (ConnectException e) {
            System.err.println("[" + context + "] Connessione rifiutata: server su porta " + port + " non disponibile?");
        } catch (IOException e) {
            System.err.println("[" + context + "] Errore I/O: " + e.getMessage());
        } catch (Exception e) {
            System.err.println("[" + context + "] Errore: " + e.getMessage());
        } finally {
            if (conn != null) {
                conn.disconnect();
            }
        }
        return null;
    }
}

class PrimeResult {
    final int count;
    final List<Integer> primes;
    
    PrimeResult(int count, List<Integer> primes) {
        this.count = count;
        this.primes = primes;
    }
}