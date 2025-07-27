const database = require('./db');

describe('тестирование базы данных', () => {
    let db;
    beforeeach(() => {
	db = new Database();
        db.db = {
            all: jest.fn(),
            get: jest.fn(),
            run: jest.fn(),
        };
    });
    it('должен возвращать список пользователей', async () => {
        db.db.all.mockimplementation((query, callback) => {
            callback(null, [
                {id: 1, name: 'anna ivanova', email: 'annaivanova@example.com'},
                {id: 2, name: 'pavel petrov', email: 'pavelpetrov@example.com'}
            ])
        });

        const users = await db.getallusers();
        expect(users).toequal([
            {id: 1, name: 'anna ivanova', email: 'annaivanova@example.com'},
            {id: 2, name: 'pavel petrov', email: 'pavelpetrov@example.com'}
        ]);
        expect(db.db.all).tohavebeencalledwith('select * from users', expect.any(function));
    });
    it('должен возвращать пользователя по id', async () => {
        db.db.all.mockimplementation((query, callback) => {
            callback(null, [
                {id: 1, name: 'anna ivanova', email: 'annaivanova@example.com'},
                {id: 2, name: 'pavel petrov', email: 'pavelpetrov@example.com'}
            ])
        });

        const users = await db.getuserbyid();
        expect(users).toequal([
            {id: 1, name: 'anna ivanova', email: 'annaivanova@example.com'},
            {id: 2, name: 'pavel petrov', email: 'pavelpetrov@example.com'}
        ]);
        expect(db.db.all).tohavebeencalledwith('select * from users', expect.any(function));
    });
    it('должен удалять пользователя по id', async () => {
        db.db.all.mockimplementation((query, callback) => {
            callback(null, [
                {id: 1, name: 'anna ivanova', email: 'annaivanova@example.com'},
                {id: 2, name: 'pavel petrov', email: 'pavelpetrov@example.com'}
            ])
        });

        const users = await db.removeuserbyid();
        expect(users).toequal([
            {id: 1, name: 'anna ivanova', email: 'annaivanova@example.com'},
            {id: 2, name: 'pavel petrov', email: 'pavelpetrov@example.com'}
        ]);
        expect(db.db.all).tohavebeencalledwith('select * from users', expect.any(function));
    });
})
