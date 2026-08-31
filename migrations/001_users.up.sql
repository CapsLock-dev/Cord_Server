-- migrate:up
CREATE TABLE users (
	id BIGSERIAL PRIMARY KEY,
	display_name TEXT,
	username TEXT UNIQUE NOT NULL,
	password_hash TEXT NOT NULL, -- argon2id embeds the salt, no separate column needed
	icon TEXT,
	created_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

-- migrate:down
DROP TABLE users;
