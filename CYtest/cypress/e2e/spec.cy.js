describe('Покупки', () => {
  it('Делаем покупку первого предмета в списке', () => {
    // Открываем сайт
    cy.visit('https://www.saucedemo.com/')

    // Авторизуемся
    cy.get('[data-test="username"]').type('standard_user', { delay: 200 })
    cy.get('[data-test="password"]').type('secret_sauce', { delay: 200 })
    cy.get('[data-test="login-button"]').click()
    cy.wait(1000)

    // Проверяем, что url изменился
    cy.url().should('include', '/inventory')

    // Делаем клик по первому предмету в списке
    cy.get('[data-test="add-to-cart-sauce-labs-backpack"]').click()
    cy.wait(1000)

    // Проверяем, что значок на корзине = 1
    cy.get('[data-test="shopping-cart-badge"]').invoke('text').should('equal', '1')

    // Переходим в корзину
    cy.get('[data-test="shopping-cart-link"]').click()
    cy.wait(1000)

    // Проверяем наименование и цену товара
    cy.get('[data-test="inventory-item-name"]').invoke('text').should('equal', 'Sauce Labs Backpack')
    cy.get('[data-test="inventory-item-price"]').invoke('text').should('equal', '$29.99')

    // Переходим в чекаут
    cy.get('[data-test="checkout"]').click()
    cy.wait(1000)

    // Вводим данные для доставки
    cy.get('[data-test="firstName"]').type('TEST', { delay: 200 })
    cy.get('[data-test="lastName"]').type('USER', { delay: 200 })
    cy.get('[data-test="postalCode"]').type('12345567', { delay: 200 })

    // Нажимем продолжить
    cy.get('[data-test="continue"]').click()
    cy.wait(1000)

    // Нажимем завершить
    cy.get('[data-test="finish"]').click()
    cy.wait(1000)

    // Проверяем благодарность за покупку
    cy.get('[data-test="complete-header"]').invoke('text').should('equal', 'Thank you for your order!')
  })
})
