describe('Testing ngs.ru', () => {
    it('Test ngs menu button', () => {
        cy.visit('https://www.ngs.ru/');

        cy.get('[aria-label="Menu Toggle"]').click();

        cy.get('a[href="/text/auto/"]').first().click();

        cy.url().should('include', '/text/auto/');
    });
});
