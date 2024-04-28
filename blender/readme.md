# blender importer

## setup:

1. install Python 3.10.6
2. prepare env:

    ```
    python -m venv ./.venv
    .\.venv\Scripts\Activate.ps1
    pip install -r requirements.txt
    ```

    or using vscode: `Ctrl+Shift+P` -> `Python: Create Env`

4. when installing deps:

    ```
    pip install package && pip freeze > requirements.txt
    ```

## usage:

```
python .\import.py `
  --input "C:/Users/mnz-r/Desktop/3dmodels/dae-diorama-forest-loner-racing/dae-diorama-forest-loner-racing.blend" `
  --output "E:/src/vydra/nerv/blender/tmp"
```

```
python .\demo.py --input "C:\src\resources\X0\MR1F-MFA\mr1f-pp.blend" --output demo-data
```
