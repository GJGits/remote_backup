// iniziale codice da inserire per avere una connessione, statement e risultato

    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;
    std::shared_ptr<sql::Connection> con;
    
// La funzione selezionare il corretto id evitando buchi nel mezzo    
    
    unsigned int retrieve_id() {
    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;
    std::shared_ptr<sql::Connection> con;
    con = DBConnect::getConnection();
    stmt = std::unique_ptr<sql::PreparedStatement>{std::move( con->prepareStatement("SELECT id from users;"))};
    res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery())};
    unsigned int actual = 0;
    unsigned int past = 0;
    if(res->next()){
    	while(1){
	    	actual = res->getUInt(1);
	    	if((actual - past) > 1) // c'è un foto
	    		return past + 1; //inserisco l'elemento nel foro	
	    	past = actual;
	    	if(!res->next())  //se sono tutti pieni e finisce, ritorna il max trovato + 1
	    		return actual + 1;
	    }
    }
    else
    	return 0;
    	
}
    
// Per la INSERT

    con = DBConnect::getConnection();
    stmt = std::unique_ptr<sql::PreparedStatement>{std::move( con->prepareStatement("INSERT INTO users(id, username, password, salt) VALUES(DEFAULT, ?, ?, ?)"))};
    stmt->setString(1, "Test_username");
    stmt->setString(2, "0001");
    stmt->setInt(3, 5);
    stmt->execute();
    
    
// Per la SELECT con stampa della stringa ottenuta come risultato

    con = DBConnect::getConnection();
    stmt = std::unique_ptr<sql::PreparedStatement>{std::move( con->prepareStatement("SELECT username FROM users WHERE username = ?"))};
    stmt->setString(1, "Test_username");
    res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery())};
    res->next();
    std::string username = std::string { res->getString("username").c_str() };
    std::clog << "username: " << username << "\n";
    
    
// Per la DELETE

    con = DBConnect::getConnection();
    stmt = std::unique_ptr<sql::PreparedStatement>{std::move( con->prepareStatement("DELETE FROM users WHERE username = ?"))};
    stmt->setString(1, "Test_username");
    stmt->execute();
