--
-- Name: packages; Type: TABLE
--

CREATE TABLE packages (
    uid integer NOT NULL,
    name character varying(96) NOT NULL,
    keywords character varying(32)[],
    location character varying(128) NOT NULL,
    checksum character(40) NOT NULL,
    url character varying(128),
    active boolean DEFAULT true NOT NULL,
    author character varying(64) NOT NULL,
    license character varying(8),
    description text
);

--
-- Name: packages_uid_seq; Type: SEQUENCE
--

CREATE SEQUENCE packages_uid_seq
    START WITH 1000
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

--
-- Name: packages_uid_seq; Type: SEQUENCE OWNED BY
--

ALTER SEQUENCE packages_uid_seq OWNED BY packages.uid;

--
-- Name: settings; Type: TABLE
--

CREATE TABLE settings (
    key character varying(32) NOT NULL,
    value text,
    active boolean DEFAULT true NOT NULL
);

--
-- Name: uid; Type: DEFAULT
--

ALTER TABLE ONLY packages ALTER COLUMN uid SET DEFAULT nextval('packages_uid_seq'::regclass);

--
-- Name: settings_pkey; Type: CONSTRAINT
--

ALTER TABLE ONLY settings
    ADD CONSTRAINT settings_pkey PRIMARY KEY (key);
