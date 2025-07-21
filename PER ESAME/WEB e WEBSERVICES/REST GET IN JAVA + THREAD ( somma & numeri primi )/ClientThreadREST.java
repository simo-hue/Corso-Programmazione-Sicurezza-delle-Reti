/*

    Devo modificare l'indirizzo a riga 19-21 per distribuirlo su più server.

*/
// -----------------------------------------------------------------------------
// Compilazione:
//     java ClientThreadREST numeri-primi 1 50
// -----------------------------------------------------------------------------
import java.io.*; 
import java.net.*;
import java.util.*;
import java.util.concurrent.*;

class ClientThreadREST
{    
    // Array degli indirizzi IP dei server disponibili
    private static final String[] SERVERS = {
        "192.168.1.100", // Server 1
        "192.168.1.101", // Server 2  
        "192.168.1.102"  // Server 3
    };
    
    public static void main(String args[])
    {
        if(args.length < 3) {
            System.out.println("USAGE: java ClientThreadREST tipofunzione op1 op2");
            return;
        }
        
        String service = args[0];
        int start = Integer.parseInt(args[1]);
        int end = Integer.parseInt(args[2]);
        
        if(service.equals("numeri-primi")) {
            calcolaNumeriPrimiDistribuito(start, end);
        } else if(service.equals("calcola-somma")) {
            // Per la somma non serve distribuzione
            RESTAPI service1 = new RESTAPI(SERVERS[0], service, args[1], args[2]);
            service1.start();
            try {
                service1.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        } else {
            System.out.println("Servizio non supportato: " + service);
        }
    }
    
    private static void calcolaNumeriPrimiDistribuito(int start, int end) {
        int numServers = SERVERS.length;
        int range = end - start + 1;
        int rangePerServer = range / numServers;
        int remainder = range % numServers;
        
        List<RESTAPI> threads = new ArrayList<>();
        Set<Integer> tuttiIPrimi = Collections.synchronizedSet(new TreeSet<>());
        
        System.out.println("Distribuendo calcolo su " + numServers + " server...");
        
        int currentStart = start;
        for(int i = 0; i < numServers; i++) {
            int currentEnd = currentStart + rangePerServer - 1;
            // Distribuisci il remainder sui primi server
            if(i < remainder) currentEnd++;
            
            System.out.println("Server " + SERVERS[i] + ": intervallo [" + 
                             currentStart + ", " + currentEnd + "]");
            
            RESTAPI thread = new RESTAPI(SERVERS[i], "numeri-primi", 
                                       String.valueOf(currentStart), 
                                       String.valueOf(currentEnd), 
                                       tuttiIPrimi);
            threads.add(thread);
            thread.start();
            
            currentStart = currentEnd + 1;
        }
        
        // Aspetta che tutti i thread terminino
        for(RESTAPI thread : threads) {
            try {
                thread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        
        // Mostra risultati finali
        System.out.println("\n=== RISULTATI FINALI ===");
        System.out.println("Totale numeri primi trovati: " + tuttiIPrimi.size());
        System.out.println("Numeri primi: " + tuttiIPrimi);
    }
}

class RESTAPI extends Thread
{
    String server, service, param1, param2;
    Set<Integer> risultatiCondivisi;

    public void run() {
        if(service.equals("calcola-somma")) {
            System.out.println("Risultato: " + calcolaSomma(Float.parseFloat(param1), Float.parseFloat(param2)));
        }
        else if(service.equals("numeri-primi")) {
            List<Integer> primi = calcolaNumeriPrimi(Integer.parseInt(param1), Integer.parseInt(param2));
            if(risultatiCondivisi != null) {
                risultatiCondivisi.addAll(primi);
            }
            System.out.println("Server " + server + " ha trovato " + primi.size() + " numeri primi");
        }
        else {
            System.out.println("Servizio non disponibile!");
        }
    }

    // Costruttore per somma
    RESTAPI(String remoteServer, String srvc, String p1, String p2) {
        server = new String(remoteServer);
        service = new String(srvc);
        param1 = new String(p1);
        param2 = new String(p2);
    }
    
    // Costruttore per numeri primi con risultati condivisi
    RESTAPI(String remoteServer, String srvc, String p1, String p2, Set<Integer> risultati) {
        this(remoteServer, srvc, p1, p2);
        this.risultatiCondivisi = risultati;
    }

    synchronized float calcolaSomma(float val1, float val2) {
        URL u = null;
        float risultato = 0;
        
        try {
            u = new URL("http://" + server + ":8000/calcola-somma?param1=" + val1 + "&param2=" + val2);
            System.out.println("URL aperto: " + u);
            
            URLConnection c = u.openConnection();
            c.connect();
            BufferedReader b = new BufferedReader(new InputStreamReader(c.getInputStream()));
            
            String s;
            while((s = b.readLine()) != null) {
                System.out.println(s);
                int i = s.indexOf("somma");
                if(i != -1) {
                    risultato = Float.parseFloat(s.substring(i + 7));
                }
            }
        }
        catch (Exception e) {
            System.out.println("Errore connessione server " + server + ": " + e.getMessage());
        }
    
        return risultato;
    }
    
    synchronized List<Integer> calcolaNumeriPrimi(int start, int end) {
        List<Integer> primi = new ArrayList<>();
        URL u = null;
        
        try {
            u = new URL("http://" + server + ":8000/numeri-primi?param1=" + start + "&param2=" + end);
            System.out.println("URL aperto: " + u);
            
            URLConnection c = u.openConnection();
            c.connect();
            BufferedReader b = new BufferedReader(new InputStreamReader(c.getInputStream()));
            
            StringBuilder response = new StringBuilder();
            String s;
            while((s = b.readLine()) != null) {
                response.append(s);
            }
            
            // Parsing semplice del JSON (in produzione usare una libreria JSON)
            String jsonResponse = response.toString();
            System.out.println("Risposta da " + server + ": " + jsonResponse);
            
            // Estrai i numeri primi dall'array JSON
            int startBracket = jsonResponse.indexOf("[");
            int endBracket = jsonResponse.lastIndexOf("]");
            
            if(startBracket != -1 && endBracket != -1 && startBracket < endBracket) {
                String primiStr = jsonResponse.substring(startBracket + 1, endBracket);
                if(!primiStr.trim().isEmpty()) {
                    String[] primiArray = primiStr.split(",");
                    for(String primo : primiArray) {
                        primi.add(Integer.parseInt(primo.trim()));
                    }
                }
            }
            
        }
        catch (Exception e) {
            System.out.println("Errore connessione server " + server + ": " + e.getMessage());
        }
        
        return primi;
    }
}