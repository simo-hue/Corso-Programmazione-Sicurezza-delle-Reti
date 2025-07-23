//Client

var name = prompt("What's your name?");
while(name == "" || name == null) {
    name = prompt("You have to choose a name. \n What's your name?");
}

// Query DOM
var message = document.getElementById('messaggio'),
   sender = document.getElementById('sender'),
   btn = document.getElementById('send'),
   output = document.getElementById('output'),
   feedback = document.getElementById('feedback');

sender.innerHTML = name;  
sender.value = name;

// Invio richiesta di connessione al server (Socket.IO v2)
var socket = io.connect();

// Gestione connessione
socket.on('connect', function() {
    console.log('Connesso al server!');
});

// Listen for events
btn.addEventListener('click', function(){
   if (message.value != "") {    
       console.log('Invio messaggio:', message.value);
       socket.emit('messaggio', {
        message: message.value,
        sender: sender.value,
       });
       message.value = "";
   }
});

message.onkeydown = function(e){
   e = e || window.event;
   socket.emit('typing', {
      sender: sender.value
   });
}

socket.on('aggiornamento', function(data){
   console.log('Messaggio ricevuto:', data);
   feedback.innerHTML = '';
   output.innerHTML += '<p><strong>' + data.sender + ': </strong>' + data.message + '</p>';
   output.scrollTop = output.scrollHeight;
});

socket.on('typing', function(data) {
   feedback.innerHTML = '<p><em>' + data.sender + ' is typing a message...</em></p>';
});