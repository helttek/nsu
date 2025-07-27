const sqlite3 = require('sqlite3').verbose();

class Database {

    constructor(dbPath = ':memory:') {
        this.db = new sqlite3.Database(dbPath);
    }

    async getAllUsers() {
        return new Promise((resolve, reject) => {
            this.db.all('SELECT * FROM users', (err, rows) => {
                if (err) reject(err);
                else resolve(rows);
            });
        });
    }

    //async getUserById(id){
    //    return new Promise((resolve, reject) => {
    //        this.db.all('SELECT * FROM users where id = ?', [id], (err, rows) => {
    //            if (err) reject(err);
    //            else resolve(rows);
    //        });
    //    });
    //}
    //async removeUserById(id){
    //    return new Promise((resolve, reject) => {
    //        this.db.all('Delete * FROM users where id = ?', [id], (err, rows) => {
    //            if (err) reject(err);
    //            else resolve(rows);
    //        });
    //    });
    //}
}
