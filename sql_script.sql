-- Criando a tabela tbusuario
create database dbcommandshield;

use dbcommandshield;

select * from  tbusuario;
CREATE TABLE tbusuario (
    idusuario INT AUTO_INCREMENT PRIMARY KEY,  
    login VARCHAR(100) NOT NULL,
    senha VARCHAR(100) NOT NULL
);

-- Criando a tabela tbfunction
CREATE TABLE tbfunction (
    idfunction INT AUTO_INCREMENT PRIMARY KEY,   
    descfunction VARCHAR(100) NOT NULL
);

-- Criando a tabela tblog
CREATE TABLE tblog (
    idlog INT AUTO_INCREMENT PRIMARY KEY,          
    idusuario INT,                             
    idfunction INT,                               
    dthoratualizacao DATETIME NOT NULL,           
    FOREIGN KEY (idusuario) REFERENCES tbusuario(idusuario), 
    FOREIGN KEY (idfunction) REFERENCES tbfunction(idfunction) 
);

-- Criando a view vw_historico_funcionalidades
CREATE VIEW vw_historico_funcionalidades AS
SELECT 
    u.idusuario,
    u.login,
    DATE(l.dthoratualizacao) AS dia,
    GROUP_CONCAT(f.descfunction ORDER BY l.dthoratualizacao SEPARATOR ', ') AS funcionalidades_usadas
FROM 
    tbusuario u
JOIN 
    tblog l ON l.idusuario = u.idusuario
JOIN 
    tbfunction f ON f.idfunction = l.idfunction
GROUP BY 
    u.idusuario, DATE(l.dthoratualizacao)
ORDER BY 
    dia DESC;
