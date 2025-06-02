describe('Проверка сортировки товаров', () => {
    it('Сортирует товары от Z к A и проверяет позиции', () => {
        cy.visit('https://www.saucedemo.com/');
        cy.get('[data-test="username"]').type('standard_user');
        cy.get('[data-test="password"]').type('secret_sauce');
        cy.get('[data-test="login-button"]').click();
        cy.wait(1000);

        cy.url().should('include', '/inventory');

        cy.get('.inventory_item').first().within(() => {
            cy.get('.inventory_item_name').should('have.text', 'Sauce Labs Backpack');
            cy.get('.inventory_item_price').should('have.text', '$29.99');
        });

        cy.get('[data-test="product-sort-container"]').select('Name (Z to A)');
        cy.wait(1000);

        cy.get('.inventory_item').first().within(() => {
            cy.get('.inventory_item_name').should('have.text', 'Test.allTheThings() T-Shirt (Red)');
            cy.get('.inventory_item_price').should('have.text', '$15.99');
        });

        cy.get('.inventory_item').last().within(() => {
            cy.get('.inventory_item_name ').should('have.text', 'Sauce Labs Backpack');
            cy.get('.inventory_item_price').should('have.text', '$29.99');
        });
    });
});
