describe('Добавление и удаление товара из корзины', () => {
    it('Добавляет и удаляет Sauce Labs Bolt T-Shirt', () => {
        cy.visit('https://www.saucedemo.com/');
        cy.get('[data-test="username"]').type('problem_user');
        cy.get('[data-test="password"]').type('secret_sauce');
        cy.get('[data-test="login-button"]').click();
        cy.wait(1000);

        cy.url().should('include', '/inventory');

        cy.contains('.inventory_item_name', 'Sauce Labs Bolt T-Shirt')
            .parents('.inventory_item')
            .within(() => {
                cy.get('button').click();
                cy.wait(1000);
            });

        cy.get('.shopping_cart_badge').should('have.text', '1');

        cy.contains('.inventory_item_name', 'Sauce Labs Bolt T-Shirt')
            .parents('.inventory_item')
            .within(() => {
                cy.get('button').click();
                cy.wait(1000);
            });

        cy.get('.shopping_cart_badge').should('not.exist');
    });
});
