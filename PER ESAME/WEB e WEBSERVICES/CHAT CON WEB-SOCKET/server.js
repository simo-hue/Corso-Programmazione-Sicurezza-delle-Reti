//Server: Fare partire con: nodemon server.js

var express = require('express');
var socket = require('socket.io');

//Chat setup
var app = express();
// in questo momento il server è in attesa delle connessioni HTTP sulla porta specificata
var porta = 4000
var server = app.listen(porta, function(){
   console.log('Server in ascolto sulla porta 4000');
   console.log('Vai su http://localhost:', porta);
});

// Static files
/*con questa funzione viene specificato a Nodejs che
una volta ricevuta una connessione deve andare a
cercare nella cartella public il file html da fornire
al client
*/
app.use(express.static('public'));

// Socket setup & pass server
/*una volta che la connessione è stata ricevuta qui
qui viene effettuato l'upgrade ad una connessione
websocket e il server si mette in attesa degli
eventi ai quali rispondere
*/

var io = socket(server);
io.on('connection', function(webSocket){
   console.log('Arrivata una connessione HTTP, attendo upgrade a WebSocket');
   console.log('made webSocket connection', webSocket.id);

   // Ricezione di un messaggio da inoltrare ai client
   webSocket.on('messaggio', function(data){
       console.log('Messaggio ricevuto:', data);
       io.sockets.emit('aggiornamento', data);
   });

   // Ricezione di un messaggio che l'utente sta scrivendo ( aggiornamento da inoltrare ai client )
   webSocket.on('typing', function(data) {
      webSocket.broadcast.emit('typing',data);
   });
});