class TaskManager {
  constructor() {
    this.tasks = JSON.parse(localStorage.getItem('tasks')) || [];
    this.taskInput = document.getElementById('taskInput');
    this.prioritySelect = document.getElementById('prioritySelect');
    this.addTaskBtn = document.getElementById('addTaskBtn');
    this.taskList = document.getElementById('taskList');
    this.totalTasksEl = document.getElementById('totalTasks');
    this.completedTasksEl = document.getElementById('completedTasks');

    this.bindEvents();
    this.render();
  }

  bindEvents() {
    this.addTaskBtn.addEventListener('click', () => this.addTask());
    this.taskInput.addEventListener('keyup', (e) => {
      if (e.key === 'Enter') this.addTask();
    });
  }

  addTask() {
    const taskText = this.taskInput.value.trim();
    const priority = this.prioritySelect.value;

    if (taskText) {
      const newTask = {
        id: Date.now(),
        text: taskText,
        priority,
        completed: false
      };

      this.tasks.push(newTask);
      this.taskInput.value = '';
      this.saveAndRender();
    }
  }

  toggleTask(id) {
    const task = this.tasks.find(t => t.id === id);
    if (task) {
      task.completed = !task.completed;
      this.saveAndRender();
    }
  }

  deleteTask(id) {
    this.tasks = this.tasks.filter(task => task.id !== id);
    this.saveAndRender();
  }

  saveAndRender() {
    localStorage.setItem('tasks', JSON.stringify(this.tasks));
    this.render();
  }

  render() {
    this.taskList.innerHTML = '';

    this.tasks.forEach(task => {
      const taskElement = document.createElement('div');
      taskElement.classList.add('task-item');
      if (task.completed) taskElement.classList.add('completed');

      taskElement.innerHTML = `
                <div>
                    <span class="task-priority ${task.priority}">${task.priority}</span>
                    ${task.text}
                </div>
                <div class="task-actions">
                    <button onclick="taskManager.toggleTask(${task.id})">
                        ${task.completed ? '↩️' : '✔️'}
                    </button>
                    <button onclick="taskManager.deleteTask(${task.id})">🗑️</button>
                </div>
            `;

      this.taskList.appendChild(taskElement);
    });

    // Update stats
    this.totalTasksEl.textContent = `Total Tasks: ${this.tasks.length}`;
    this.completedTasksEl.textContent = `Completed: ${this.tasks.filter(t => t.completed).length}`;
  }
}

const taskManager = new TaskManager();


