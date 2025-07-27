describe('Проверка наименования и цены первого товара', () => {
    it('Проверяет название и цену в листинге и карточке товара', () => {
        cy.visit('https://www.saucedemo.com/');
        cy.get('[data-test="username"]').type('problem_user');
        cy.get('[data-test="password"]').type('secret_sauce');
        cy.get('[data-test="login-button"]').click();
        cy.wait(1000);

        cy.url().should('include', '/inventory');

        cy.get('.inventory_item').first().within(() => {
            cy.get('.inventory_item_name').should('have.text', 'Sauce Labs Backpack');
            cy.get('.inventory_item_price').should('have.text', '$29.99');
            cy.get('[data-test="item-4-title-link"]').click();
            cy.wait(1000);
        });

        cy.url().should('include', '/inventory-item.html');
        cy.get('.inventory_details_name').should('have.text', 'Sauce Labs Backpack');
        cy.get('.inventory_details_price').should('have.text', '$29.99');
    });
});
