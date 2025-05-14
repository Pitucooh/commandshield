-- Criando a tabela tbusuario
create database dbcommandshield;

use dbcommandshield;

select * from  tbusuario;
CREATE TABLE tbusuario (
    idusuario INT AUTO_INCREMENT PRIMARY KEY,  
    login VARCHAR(45) NOT NULL,
    senha VARCHAR(45) NOT NULL
);