-- Se limpian los datos
DROP TABLE IF EXISTS vendors;

CREATE TABLE
  vendors (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    phone TEXT UNIQUE NOT NULL,
    email TEXT UNIQUE NOT NULL,
    inaddr TEXT NOT NULL, -- Direccion del agente (no debe ser un campo unico en caso de que varios vendedores unifiquen una misma maquina como agente de compras)
    --
    CHECK (
      LENGTH (name) <= 128
      AND LENGTH (phone) <= 15
      AND LENGTH (email) <= 320
      AND LENGTH (inaddr) <= 15
    )
  );

INSERT INTO
  vendors (name, phone, email, inaddr)
VALUES
  (
    'Vendor1',
    '34666555444',
    'vendor1@server',
    '172.255.0.1'
  ),
  (
    'Vendor2',
    '34666555445',
    'vendor2@server',
    '172.255.0.2'
  ),
  (
    'Vendor3',
    '34666555446',
    'vendor3@server',
    '172.255.0.3'
  );

-- Se limpian los datos
DROP TABLE IF EXISTS products;

CREATE TABLE
  products (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    description TEXT NOT NULL,
    serial TEXT UNIQUE NOT NULL,
    owner TEXT NOT NULL,
    vendor_id INTEGER NOT NULL,
    price REAL DEFAULT 0 NOT NULL,
    count INTEGER DEFAULT 0 NOT NULL,
    -- Relacion N - 1
    FOREIGN KEY (vendor_id) REFERENCES vendors (id),
    --
    CHECK (
      LENGTH (name) <= 128
      AND LENGTH (description) <= 512
      AND LENGTH (serial) <= 64
      AND LENGTH (owner) <= 128
    )
  );

INSERT INTO
  products (
    name,
    description,
    serial,
    owner,
    vendor_id,
    price
  )
VALUES
  (
    'Product1',
    'Product1 description',
    '123123123',
    'Alfredo',
    1,
    3.14
  ),
  (
    'Product2',
    '234234234',
    'Mauricio',
    'Product2 description',
    1,
    6.28
  ),
  (
    'Product3',
    '345345345',
    'Paco',
    'Product3 description',
    1,
    7.99
  ),
  --
  (
    'Product4',
    '111222333',
    'Pancracio',
    'Product4 description',
    2,
    2.20
  ),
  (
    'Product5',
    '222333444',
    'Fortunatio',
    'Product5 description',
    2,
    1.99
  ),
  (
    'Product6',
    '999888777',
    'Jordi',
    'Product6 description',
    2,
    9.20
  ),
  --
  (
    'Product7',
    '987987987',
    'Cofla',
    'Product7 description',
    3,
    0.50
  ),
  (
    'Product8',
    '876876876',
    'J.Silverhand',
    'Product8 description',
    3,
    8.60
  ),
  (
    'product9',
    '123987123',
    'kelokekelowua',
    'product9 description',
    3,
    3.20
  );

DROP TABLE IF EXISTS smart_products;

CREATE TABLE
  IF NOT EXISTS smart_products (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    description TEXT NOT NULL,
    serial TEXT UNIQUE NOT NULL,
    owner TEXT NOT NULL,
    inaddr TEXT UNIQUE NOT NULL, -- No puede haber dos productos que apunten al mismo equipo
    vendor_id INTEGER NOT NULL,
    price REAL DEFAULT 0 NOT NULL,
    --
    FOREIGN KEY (vendor_id) REFERENCES vendors (id),
    CHECK (
      LENGTH (name) <= 128
      AND LENGTH (description) <= 512
      AND LENGTH (serial) <= 64
      AND LENGTH (owner) <= 128
      AND LENGTH (inaddr) <= 15 -- xxx.xxx.xxx.xxx
    )
  );

INSERT INTO
  smart_products (
    name,
    description,
    serial,
    owner,
    inaddr,
    vendor_id,
    price
  )
VALUES
  (
    'SmartProduct1',
    'SmartProduct1 Description',
    '123987123',
    'kelokekelowua',
    '172.20.167.118',
    3,
    3.20
  );

-- Vista para productos estandar
DROP VIEW IF EXISTS products_info;

CREATE VIEW
  products_info AS
SELECT
  p.id,
  p.name AS name,
  p.description,
  p.serial,
  p.owner,
  v.name AS vendor_name,
  p.count,
  p.price
FROM
  products AS p
  INNER JOIN vendors AS v ON p.vendor_id = v.id
ORDER BY
  p.price DESC;

-- Vista para productos inteligentes
DROP VIEW IF EXISTS smart_products_info;

CREATE VIEW
  smart_products_info AS
SELECT
  p.id,
  p.name AS name,
  p.description,
  p.serial,
  p.owner,
  p.inaddr,
  v.name AS vendor_name,
  p.price
FROM
  smart_products AS p
  INNER JOIN vendors AS v ON p.vendor_id = v.id
ORDER BY
  p.price DESC