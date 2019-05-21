# ATECH AIR
Projeto de código para controle de ar condicionado por IR;

Esta branch conta com as seguintes funcionalidades:

# Wifi Manager Feature
 <p> 1 - Quando não tem wifi cadastrado o device entra em modo AP;</p>
 <p> 2 - Em modo AP é apresentado uma página com um formulário para cadastrar usuário, senha e ssid, configurarando assim o wifi;</p>
 <p> 3 - Quando tem um wifi cadastrado, o device entra em modo STATION;</p>
 <p> 4 - Em modo STATION o device tenta se conectar 6 vezes consecutivas no wifi cadastrado em modo ap;</p>
 <p> 5 - Em caso de sucesso de conexão o device executa uma verificação de conexão de tempos em tempos e habilita o blynk;</p>
 <p>6 - Em caso de fracasso de conexão o device apaga o wifi cadastrado e entra em modo AP novamente;</p>


Conceito:
https://docs.google.com/presentation/d/17hxNolOvLjVx8DzgZVnKvo2QckECtDiJQxq4ciyT4sY/edit#slide=id.g58bacd00ad_0_47
