const display = document.getElementById('display');
const buttonsContainer = document.querySelector('.buttons');

let currentOperand = '';
let previousOperand = '';
let operation = null;
let shouldResetDisplay = false;

// --- Core Calculation Logic ---
function calculate() {
    let computation;
    const prev = parseFloat(previousOperand);
    const current = parseFloat(currentOperand);

    if (isNaN(prev) || isNaN(current)) return; // Exit if numbers are invalid

    switch (operation) {
        case '+':
            computation = prev + current;
            break;
        case '-':
            computation = prev - current;
            break;
        case '*':
            computation = prev * current;
            break;
        case '/':
            if (current === 0) {
                computation = 'Error'; // Handle division by zero
            } else {
                computation = prev / current;
            }
            break;
        default:
            return; // Exit if no valid operation
    }

    // Limit decimal places for display
    if (typeof computation === 'number') {
        // Avoid excessive decimals from floating point issues
         computation = parseFloat(computation.toPrecision(12));
    }


    currentOperand = computation.toString();
    operation = null; // Reset operation after calculation
    previousOperand = ''; // Clear previous operand
    shouldResetDisplay = true; // Next number input should clear display
}

// --- Button Action Handlers ---
function appendNumber(number) {
    if (currentOperand === 'Error') currentOperand = ''; // Clear error on new input
    if (number === '.' && currentOperand.includes('.')) return; // Allow only one decimal
    if (shouldResetDisplay) {
        currentOperand = '';
        shouldResetDisplay = false;
    }
    // Prevent excessively long numbers (optional)
    if (currentOperand.length > 15) return;

    currentOperand = currentOperand.toString() + number.toString();
}

function chooseOperation(selectedOperation) {
    if (currentOperand === '' && previousOperand === '') return; // Nothing to operate on
    if (currentOperand === 'Error') { // Clear error state
        clearAll();
        return;
    }
    if (previousOperand !== '') {
        // If there's a pending operation, calculate it first
        calculate();
        // Update display immediately after intermediate calculation
        updateDisplay();
    }
    // If calculation resulted in an error, stop here
     if (currentOperand === 'Error') {
        updateDisplay(); // Show the error
        operation = null;
        previousOperand = '';
        return;
    }

    operation = selectedOperation;
    previousOperand = currentOperand; // Move current to previous
    currentOperand = ''; // Ready for next operand
    shouldResetDisplay = false; // Allow continued input if needed right after operator
}

function clearAll() {
    currentOperand = '';
    previousOperand = '';
    operation = null;
    shouldResetDisplay = false;
    updateDisplay('0'); // Reset display to 0
}

function toggleSign() {
     if (currentOperand === '' || currentOperand === '0' || currentOperand === 'Error') return;
     currentOperand = (parseFloat(currentOperand) * -1).toString();
     updateDisplay();
}

function percentage() {
     if (currentOperand === '' || currentOperand === 'Error') return;
     currentOperand = (parseFloat(currentOperand) / 100).toString();
     shouldResetDisplay = true; // Treat like calculation result
     updateDisplay();
}

function handleDecimal() {
     if (shouldResetDisplay) { // If starting new number after calculation
        currentOperand = '0.';
        shouldResetDisplay = false;
     } else if (currentOperand === '' || currentOperand === 'Error') {
         currentOperand = '0.'; // Start with 0. if empty
     } else if (!currentOperand.includes('.')) {
         appendNumber('.'); // Add decimal if not present
     }
      updateDisplay();
}


// --- Display Update ---
function updateDisplay(value = null) {
    if (value !== null) {
        display.value = value; // Use provided value (e.g., for clear)
    } else if (currentOperand === 'Error') {
         display.value = 'Error';
    } else {
        // Display current operand, or previous if current is empty (waiting for input)
        display.value = currentOperand || (previousOperand ? previousOperand + (operation ? ` ${getDisplayOperator(operation)}` : '') : '0');

        // Optional: Add subtle formatting like commas (more complex)
    }
}

function getDisplayOperator(op) {
    switch (op) {
        case '/': return '÷';
        case '*': return '×';
        default: return op;
    }
}


// --- Event Listener (Event Delegation) ---
buttonsContainer.addEventListener('click', (event) => {
    if (!event.target.matches('button')) return; // Ignore clicks not on buttons

    const button = event.target;
    const action = button.dataset.action;
    const number = button.dataset.number;
    const operator = button.dataset.operator;

    button.blur(); // Remove focus from button after click

    if (number !== undefined) {
        appendNumber(number);
        updateDisplay();
    } else if (operator !== undefined) {
        chooseOperation(operator);
        updateDisplay(); // Show previous operand and operator
    } else if (action === 'decimal') {
         handleDecimal(); // Use dedicated function for clarity
    } else if (action === 'clear') {
        clearAll();
    } else if (action === 'calculate') {
        if (operation != null && previousOperand !== '' && currentOperand !== '' && currentOperand !== 'Error') {
            calculate();
            updateDisplay();
        }
    } else if (action === 'toggle-sign') {
        toggleSign();
        updateDisplay();
    } else if (action === 'percentage') {
        percentage();
        updateDisplay();
    }
});

// --- Initial State ---
updateDisplay('0'); // Show 0 initially