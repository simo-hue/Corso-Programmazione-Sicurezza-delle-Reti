import java.io.*; 
import java.net.*;
import java.util.List;
import java.util.ArrayList;

class ClientREST 
{    
    public static void main(String args[])
    {
        RESTAPI service1 = new RESTAPI("127.0.0.1");

        if(args.length < 3) {
            System.out.println("USAGE: java ClientREST tipofunzione op1 op2");
            System.out.println("Servizi disponibili:");
            System.out.println("  calcola-somma num1 num2");
            System.out.println("  calcola-primi min max");
        }   
        else if(args[0].equals("calcola-somma")) {
            // Servizio calcolo somma (esistente)
            float risultato = service1.calcolaSomma(Float.parseFloat(args[1]), Float.parseFloat(args[2]));
            System.out.println("Risultato somma: " + risultato);
        }
        else if(args[0].equals("calcola-primi")) {
            // Nuovo servizio calcolo numeri primi
            int min = Integer.parseInt(args[1]);
            int max = Integer.parseInt(args[2]);
            
            PrimeResult risultato = service1.calcolaPrimi(min, max);
            
            if(risultato != null) {
                System.out.println("=== RISULTATO CALCOLO NUMERI PRIMI ===");
                System.out.println("Intervallo: [" + risultato.min + ", " + risultato.max + "]");
                System.out.println("Numeri primi trovati: " + risultato.numeriPrimi.size());
                System.out.println("Tempo di esecuzione: " + risultato.tempoEsecuzione + " ms");
                System.out.println("Lista numeri primi: " + risultato.numeriPrimi);
            } else {
                System.out.println("Errore nel calcolo dei numeri primi");
            }
        }
        else {
            System.out.println("Servizio non riconosciuto: " + args[0]);
        }
    }
}

/**
 * Classe per contenere il risultato del calcolo dei numeri primi
 */
class PrimeResult {
    public int min;
    public int max;
    public List<Integer> numeriPrimi;
    public double tempoEsecuzione;
    
    public PrimeResult() {
        numeriPrimi = new ArrayList<>();
    }
}

class RESTAPI
{
    String server;

    RESTAPI(String remoteServer) {
        server = new String(remoteServer);
    }

    /**
     * Metodo esistente per il calcolo della somma
     */
    float calcolaSomma(float val1, float val2) {
        URL u = null;
        float risultato = 0;
        int i;

        try {
            // Nota: il server si aspetta param1 e param2, ma usa op1 e op2 internamente
            u = new URL("http://" + server + ":8000/calcola-somma?op1=" + val1 + "&op2=" + val2);
            System.out.println("URL aperto: " + u);
        }
        catch (MalformedURLException e) {
            System.out.println("URL errato: " + u);
            return -1;
        }

        try {
            URLConnection c = u.openConnection();
            c.connect();
            BufferedReader b = new BufferedReader(new InputStreamReader(c.getInputStream()));
            System.out.println("Lettura dei dati...");
            String s;
            while((s = b.readLine()) != null) {
                System.out.println(s);
                if((i = s.indexOf("somma")) != -1) {
                    // Parsing del valore dopo "somma":
                    String valoreSomma = s.substring(i + 7).replaceAll("[^0-9.-]", "");
                    risultato = Float.parseFloat(valoreSomma);
                }
            }
        }
        catch (IOException e) {
            System.out.println("Errore di I/O: " + e.getMessage());
        }
        catch (NumberFormatException e) {
            System.out.println("Errore nel parsing del numero: " + e.getMessage());
        }
    
        return risultato;
    }
    
    /**
     * Nuovo metodo per il calcolo dei numeri primi
     * @param min limite inferiore dell'intervallo
     * @param max limite superiore dell'intervallo
     * @return oggetto PrimeResult con i risultati
     */
    PrimeResult calcolaPrimi(int min, int max) {
        URL u = null;
        PrimeResult risultato = new PrimeResult();

        try {
            u = new URL("http://" + server + ":8000/calcola-primi?min=" + min + "&max=" + max);
            System.out.println("URL aperto: " + u);
        }
        catch (MalformedURLException e) {
            System.out.println("URL errato: " + u);
            return null;
        }

        try {
            URLConnection c = u.openConnection();
            c.setConnectTimeout(10000); // 10 secondi timeout
            c.setReadTimeout(30000);    // 30 secondi timeout lettura
            c.connect();
            
            BufferedReader b = new BufferedReader(new InputStreamReader(c.getInputStream()));
            System.out.println("Lettura dei dati dal server...");
            
            StringBuilder jsonResponse = new StringBuilder();
            String line;
            
            // Legge tutta la risposta JSON
            while((line = b.readLine()) != null) {
                System.out.println("RICEVUTO: " + line);
                jsonResponse.append(line);
            }
            
            b.close();
            
            String json = jsonResponse.toString();
            System.out.println("DEBUG: JSON completo ricevuto: " + json);
            
            if(json.trim().isEmpty()) {
                System.out.println("ERRORE: Risposta vuota dal server");
                return null;
            }
            
            // Verifica che sia un JSON valido
            if(!json.contains("{") || !json.contains("}")) {
                System.out.println("ERRORE: Risposta non è un JSON valido");
                return null;
            }
            
            // Parsing manuale del JSON (semplificato)
            System.out.println("DEBUG: Inizio parsing JSON...");
            
            // Estrae min e max dall'intervallo
            risultato.min = extractIntValue(json, "\"min\":");
            risultato.max = extractIntValue(json, "\"max\":");
            
            // Estrae il tempo di esecuzione
            risultato.tempoEsecuzione = extractDoubleValue(json, "\"tempo_esecuzione_ms\":");
            
            // Estrae l'array dei numeri primi
            risultato.numeriPrimi = extractPrimeNumbers(json);
            
            System.out.println("DEBUG: Parsing completato con successo");
            
        }
        catch (IOException e) {
            System.out.println("Errore di I/O: " + e.getMessage());
            e.printStackTrace();
            return null;
        }
        catch (Exception e) {
            System.out.println("Errore nel parsing della risposta: " + e.getMessage());
            e.printStackTrace();
            return null;
        }
    
        return risultato;
    }
    
    /**
     * Metodo helper per estrarre un valore intero dal JSON
     */
    private int extractIntValue(String json, String key) {
        int index = json.indexOf(key);
        if(index == -1) {
            System.out.println("DEBUG: Chiave '" + key + "' non trovata nel JSON");
            return 0;
        }
        
        int start = index + key.length();
        
        // Salta spazi bianchi dopo i due punti
        while(start < json.length() && 
              (json.charAt(start) == ' ' || json.charAt(start) == '\t')) {
            start++;
        }
        
        int end = start;
        
        // Trova la fine del numero
        while(end < json.length()) {
            char c = json.charAt(end);
            if(!Character.isDigit(c) && c != '-' && c != '+') {
                break;
            }
            end++;
        }
        
        if(start == end) {
            System.out.println("DEBUG: Nessun numero trovato dopo '" + key + "'");
            return 0;
        }
        
        String numberStr = json.substring(start, end).trim();
        System.out.println("DEBUG: Numero estratto per '" + key + "': '" + numberStr + "'");
        
        try {
            return Integer.parseInt(numberStr);
        } catch(NumberFormatException e) {
            System.out.println("DEBUG: Errore parsing numero '" + numberStr + "': " + e.getMessage());
            return 0;
        }
    }
    
    /**
     * Metodo helper per estrarre un valore double dal JSON
     */
    private double extractDoubleValue(String json, String key) {
        int index = json.indexOf(key);
        if(index == -1) {
            System.out.println("DEBUG: Chiave '" + key + "' non trovata nel JSON");
            return 0.0;
        }
        
        int start = index + key.length();
        
        // Salta spazi bianchi dopo i due punti
        while(start < json.length() && 
              (json.charAt(start) == ' ' || json.charAt(start) == '\t')) {
            start++;
        }
        
        int end = start;
        
        // Trova la fine del numero (incluso il punto decimale)
        while(end < json.length()) {
            char c = json.charAt(end);
            if(!Character.isDigit(c) && c != '.' && c != '-' && c != '+') {
                break;
            }
            end++;
        }
        
        if(start == end) {
            System.out.println("DEBUG: Nessun numero trovato dopo '" + key + "'");
            return 0.0;
        }
        
        String numberStr = json.substring(start, end).trim();
        System.out.println("DEBUG: Numero double estratto per '" + key + "': '" + numberStr + "'");
        
        try {
            return Double.parseDouble(numberStr);
        } catch(NumberFormatException e) {
            System.out.println("DEBUG: Errore parsing double '" + numberStr + "': " + e.getMessage());
            return 0.0;
        }
    }
    
    /**
     * Metodo helper per estrarre l'array dei numeri primi dal JSON
     */
    private List<Integer> extractPrimeNumbers(String json) {
        List<Integer> primes = new ArrayList<>();
        
        System.out.println("DEBUG: Parsing array numeri primi...");
        System.out.println("DEBUG: JSON completo:\n" + json);
        
        // Trova l'inizio dell'array "numeri_primi"
        int startArray = json.indexOf("\"numeri_primi\": [");
        if(startArray == -1) {
            System.out.println("DEBUG: Array 'numeri_primi' non trovato nel JSON");
            return primes;
        }
        
        int start = json.indexOf('[', startArray);
        if(start == -1) {
            System.out.println("DEBUG: Carattere '[' non trovato");
            return primes;
        }
        start++; // Salta il carattere '['
        
        int end = json.indexOf(']', start);
        if(end == -1) {
            System.out.println("DEBUG: Carattere ']' non trovato");
            return primes;
        }
        
        String arrayContent = json.substring(start, end).trim();
        System.out.println("DEBUG: Contenuto array: '" + arrayContent + "'");
        
        // Se l'array è vuoto
        if(arrayContent.isEmpty()) {
            System.out.println("DEBUG: Array vuoto");
            return primes;
        }
        
        // Splitta i numeri e li aggiunge alla lista
        String[] numbers = arrayContent.split(",");
        System.out.println("DEBUG: Numero di elementi da parsare: " + numbers.length);
        
        for(int i = 0; i < numbers.length; i++) {
            String num = numbers[i].trim();
            System.out.println("DEBUG: Parsing numero " + i + ": '" + num + "'");
            
            if(!num.isEmpty()) {
                try {
                    int primo = Integer.parseInt(num);
                    primes.add(primo);
                    System.out.println("DEBUG: Aggiunto numero primo: " + primo);
                } catch(NumberFormatException e) {
                    System.out.println("DEBUG: Errore nel parsing del numero primo '" + num + "': " + e.getMessage());
                }
            }
        }
        
        System.out.println("DEBUG: Totale numeri primi estratti: " + primes.size());
        return primes;
    }
}