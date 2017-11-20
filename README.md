Para desenvolver a plataforma de experimentos, pensou-se em criar uma hierarquia agentes no sistema. O manager será programa em C e deverá ser executado em um computador Linux. O manager deverá ler um arquivo XML contendo configurações para o experimento a ser realizado. Ele então enviará, via Serial, estes dados para um mote MICAZ que assumirá o papel de coordenador na hierarquia. O coordenador então será responsável por identificar os motes ativos na rede, configurá-los e ao fim de cada experimento receber medidas realizadas por estes motes e repassa-las ao manager.

Como resultado desta primeira parte do desenvolvimento da plataforma de experimentos obteve-se a comunicação entre manager, coordenador e os motes ativos na rede. Assim sendo, ao utilizar a plataforma de experimentos, será possível receber status dos motes que compõem a rede. Isso será feito através do coordenador conectado na serial do PC. Os seguintes diagrama mostram o funcionamento do protocolo, ou seja, a interação entrele PC (manager), Coordenador e Motes.

Máquina de estados do manager
![alt text](https://raw.githubusercontent.com/AndreFWeber/PlataformaADS/master/doc/FSM_Manager_LeStatus.png)

Máquina de estados do coordenador
![alt text](https://raw.githubusercontent.com/AndreFWeber/PlataformaADS/master/doc/FSM_Coordinator_LeStatus.png)
