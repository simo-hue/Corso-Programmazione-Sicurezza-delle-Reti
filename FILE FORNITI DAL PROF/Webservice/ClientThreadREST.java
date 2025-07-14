import java.io.*; 
import java.net.*;

class ClientThreadREST
{    
    public static void main(String args[])
    {
        if(args.length < 3)    {
            System.out.println("USAGE: java ClientREST tipofunzione op1 op2");
        }   
        else  {
            RESTAPI service1=new RESTAPI("127.0.0.1", args[0], args[1], args[2]);
            RESTAPI service2=new RESTAPI("127.0.0.1", args[0], args[1], args[2]);
            RESTAPI service3=new RESTAPI("127.0.0.1", args[0], args[1], args[2]);
            service1.start();
            service2.start();
            service3.start();
        }
    }
}

class RESTAPI extends Thread
{
    String server, service, param1, param2;

    public void run()   {
        if(service.equals("calcola-somma"))    {
            System.out.println("Risultato: " + calcolaSomma(Float.parseFloat(param1), Float.parseFloat(param2)));
        }
        else    {
            System.out.println("Servizio non disponibile!");
        }

    }

    RESTAPI(String remoteServer, String srvc, String p1, String p2)  {
        server = new String(remoteServer);
        service = new String(srvc);
        param1 = new String(p1);
        param2 = new String(p2);
    }

    synchronized float calcolaSomma(float val1, float val2)  {

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

}
