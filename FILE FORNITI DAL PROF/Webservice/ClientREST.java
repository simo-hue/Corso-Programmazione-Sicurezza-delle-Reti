import java.io.*; 
import java.net.*;

class ClientREST 
{    
    public static void main(String args[])
    {
        RESTAPI service1=new RESTAPI("127.0.0.1");

        if(args.length < 3)    {
            System.out.println("USAGE: java ClientREST tipofunzione op1 op2");
            System.out.println("Funzioni disponibili:");
            System.out.println("  - calcola-somma NUM1 NUM2");
            System.out.println("  - numeri-primi MIN MAX");
        }   
        else if(args[0].equals("calcola-somma")) {
            System.out.println("Risultato: " + service1.calcolaSomma(Float.parseFloat(args[1]), Float.parseFloat(args[2])));
        }
        else if(args[0].equals("numeri-primi")) {
            String risultato = service1.numeriPrimi(Integer.parseInt(args[1]), Integer.parseInt(args[2]));
            System.out.println("Numeri primi nell'intervallo [" + args[1] + ", " + args[2] + "]:");
            System.out.println(risultato);
        }
        else {
            System.out.println("Funzione non riconosciuta: " + args[0]);
            System.out.println("Funzioni disponibili: calcola-somma, numeri-primi");
        }
    }
}

class RESTAPI
{
    String server;

    RESTAPI(String remoteServer)  {
        server = new String(remoteServer);
    }

    float calcolaSomma(float val1, float val2)  {

        URL u = null;
        float risultato=0;
        int i;

        try 
        { 
            u = new URL("http://"+server+":8000/calcola-somma?param1="+val1+"&param2="+val2);
            System.out.println("URL aperto: " + u);
        }
        catch (MalformedURLException e) 
        {
            System.out.println("URL errato: " + u);
            return 0;
        }

        try 
        {
            URLConnection c = u.openConnection();
            c.connect();
            BufferedReader b = new BufferedReader(new InputStreamReader(c.getInputStream()));
            System.out.println("Lettura dei dati...");
            String s;
            while( (s = b.readLine()) != null ) {
                System.out.println(s);
                if((i = s.indexOf("somma"))!=-1)
                    risultato = Float.parseFloat(s.substring(i+7));
            }
        }
        catch (IOException e) 
        {
            System.out.println(e.getMessage());
        }
    
        return (float)risultato;
    }    

    String numeriPrimi(int min, int max)  {

        URL u = null;
        StringBuilder risultato = new StringBuilder();
        
        try 
        { 
            u = new URL("http://"+server+":8000/numeri-primi?param1="+min+"&param2="+max);
            System.out.println("URL aperto: " + u);
        }
        catch (MalformedURLException e) 
        {
            System.out.println("URL errato: " + u);
            return "Errore nella formazione dell'URL";
        }

        try 
        {
            URLConnection c = u.openConnection();
            c.connect();
            BufferedReader b = new BufferedReader(new InputStreamReader(c.getInputStream()));
            System.out.println("Lettura dei dati...");
            String s;
            boolean inBody = false;
            
            while( (s = b.readLine()) != null ) {
                System.out.println(s);
                // Iniziamo a leggere il corpo della risposta dopo la riga vuota
                if(s.trim().isEmpty() && !inBody) {
                    inBody = true;
                    continue;
                }
                if(inBody) {
                    risultato.append(s).append("\n");
                }
            }
            b.close();
        }
        catch (IOException e) 
        {
            System.out.println("Errore di I/O: " + e.getMessage());
            return "Errore nella comunicazione con il server";
        }
    
        return risultato.toString();
    }    
}